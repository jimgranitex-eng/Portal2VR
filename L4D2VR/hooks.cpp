#include "hooks.h"
#include "game.h"
#include "texture.h"
#include "sdk.h"
#include "sdk_server.h"
#include "vr.h"
#include "offsets.h"
#include "sounds.h"
#include <iostream>

	Hooks::Hooks(Game *game)
{
	if (MH_Initialize() != MH_OK)
	{
		Game::errorMsg("Failed to init MinHook");
	}

	m_Game = game;
	m_VR = m_Game->m_VR;

	m_PushHUDStep = -999;
	m_PushedHud = true;

	initSourceHooks();

	// Hook IEngineSound::EmitSound via vtable
	if (m_Game->m_EngineSound)
	{
		auto** vtable = *reinterpret_cast<void***>(m_Game->m_EngineSound);
		hkEmitSound.createHook(vtable[2], &dEmitSound);
		hkEmitSound.enableHook();
	}

	hkCalcViewModelView.enableHook();
	hkS_UpdateListener.enableHook();

	hkProcessUsercmds.enableHook();
	hkReadUsercmd.enableHook();

	// Not needed: WriteUsercmdDeltaToBuffer internally calls WriteUsercmd
	// for every command, so dWriteUsercmd already catches all writes.
	//hkWriteUsercmdDeltaToBuffer.enableHook();
	hkWriteUsercmd.enableHook();

	hkCreateMove.enableHook();
	hkEyePosition.enableHook();
	hkRenderView.enableHook();


	hkWeapon_ShootPosition.enableHook();
	hkTraceFirePortal.enableHook();

	hkDrawSelf.enableHook();
	hkPlayerPortalled.enableHook();

    hkComputeError.enableHook();
    hkUpdateObject.enableHook();
    hkUpdateObjectVM.enableHook();
    hkRotateObject.enableHook();
    hkEyeAngles.enableHook();
    hkDrawModelExecute.enableHook();

	hkGetDefaultFOV.enableHook();
	hkGetFOV.enableHook();
	hkGetViewModelFOV.enableHook();

	hkSetDrawOnlyForSplitScreenUser.enableHook();
	hkPrecache.enableHook();
	hkCHudCrosshair_ShouldDraw.enableHook();
}

Hooks::~Hooks()
{
	if (MH_Uninitialize() != MH_OK)
	{
		Game::errorMsg("Failed to uninitialize MinHook");
	}
}


int Hooks::initSourceHooks()
{
	auto pRenderViewVFunc = reinterpret_cast<LPVOID>(m_Game->m_Offsets->RenderView.address);
	hkRenderView.createHook(pRenderViewVFunc, &dRenderView);

	auto calcViewModelViewAddr = reinterpret_cast<LPVOID>(m_Game->m_Offsets->CalcViewModelView.address);
	hkCalcViewModelView.createHook(calcViewModelViewAddr, &dCalcViewModelView);

	auto ProcessUsercmdsAddr = reinterpret_cast<LPVOID>(m_Game->m_Offsets->ProcessUsercmds.address);
	hkProcessUsercmds.createHook(ProcessUsercmdsAddr, &dProcessUsercmds);

	auto ReadUserCmdAddr = reinterpret_cast<LPVOID>(m_Game->m_Offsets->ReadUserCmd.address);
	hkReadUsercmd.createHook(ReadUserCmdAddr, &dReadUsercmd);

	auto WriteUsercmdAddr = reinterpret_cast<LPVOID>(m_Game->m_Offsets->WriteUsercmd.address);
	hkWriteUsercmd.createHook(WriteUsercmdAddr, &dWriteUsercmd);

	auto EyePositionAddr = reinterpret_cast<LPVOID>(m_Game->m_Offsets->EyePosition.address);
	hkEyePosition.createHook(EyePositionAddr, &dEyePosition);

    auto DrawModelExecuteAddr = reinterpret_cast<LPVOID>(m_Game->m_Offsets->DrawModelExecute.address);
    hkDrawModelExecute.createHook(DrawModelExecuteAddr, &dDrawModelExecute);

	auto PushRenderTargetAddr = reinterpret_cast<LPVOID>(m_Game->m_Offsets->PushRenderTargetAndViewport.address);
	hkPushRenderTargetAndViewport.createHook(PushRenderTargetAddr, &dPushRenderTargetAndViewport);

	auto PopRenderTargetAddr = reinterpret_cast<LPVOID>(m_Game->m_Offsets->PopRenderTargetAndViewport.address);
	hkPopRenderTargetAndViewport.createHook(PopRenderTargetAddr, &dPopRenderTargetAndViewport);

	auto VGui_PaintAddr = reinterpret_cast<LPVOID>(m_Game->m_Offsets->VGui_Paint.address);
	hkVgui_Paint.createHook(VGui_PaintAddr, &dVGui_Paint);

	auto PrePushRenderTargetAddr = reinterpret_cast<LPVOID>(m_Game->m_Offsets->PrePushRenderTarget.address);
	hkPrePushRenderTarget.createHook(PrePushRenderTargetAddr, &dPrePushRenderTarget);

	auto Weapon_ShootPositionAddr = reinterpret_cast<LPVOID>(m_Game->m_Offsets->Weapon_ShootPosition.address);
	hkWeapon_ShootPosition.createHook(Weapon_ShootPositionAddr, &dWeapon_ShootPosition);
	
	auto TraceFirePortalAddr = reinterpret_cast<LPVOID>(m_Game->m_Offsets->TraceFirePortalServer.address);
	hkTraceFirePortal.createHook(TraceFirePortalAddr, &dTraceFirePortal);

	auto DrawSelfAddr = reinterpret_cast<LPVOID>(m_Game->m_Offsets->DrawSelf.address);
	hkDrawSelf.createHook(DrawSelfAddr, &dDrawSelf);
	
	auto ClipTransformAddr = reinterpret_cast<LPVOID>(m_Game->m_Offsets->ClipTransform.address);
	hkClipTransform.createHook(ClipTransformAddr, &dClipTransform);

	// Portalling
	auto PlayerPortalledAddr = reinterpret_cast<LPVOID>(m_Game->m_Offsets->PlayerPortalled.address);
	hkPlayerPortalled.createHook(PlayerPortalledAddr, &dPlayerPortalled);

	UTIL_Portal_FirstAlongRay = reinterpret_cast<tUTIL_Portal_FirstAlongRay>(m_Game->m_Offsets->UTIL_Portal_FirstAlongRay.address);
	UTIL_IntersectRayWithPortal = reinterpret_cast<tUTIL_IntersectRayWithPortal>(m_Game->m_Offsets->UTIL_IntersectRayWithPortal.address);
	UTIL_Portal_AngleTransform = reinterpret_cast<tUTIL_Portal_AngleTransform>(m_Game->m_Offsets->UTIL_Portal_AngleTransform.address);

	auto CreateMoveAddr = reinterpret_cast<LPVOID>(m_Game->m_Offsets->CreateMove.address);
	hkCreateMove.createHook(CreateMoveAddr, &dCreateMove);

	// Sound
	auto S_UpdateListenerAddr = reinterpret_cast<LPVOID>(m_Game->m_Offsets->S_UpdateListener.address);
	hkS_UpdateListener.createHook(S_UpdateListenerAddr, &dS_UpdateListener);

	// Grababbles
	hkComputeError.createHook(reinterpret_cast<LPVOID>(m_Game->m_Offsets->ComputeError.address), &dComputeError);
	hkUpdateObject.createHook(reinterpret_cast<LPVOID>(m_Game->m_Offsets->UpdateObject.address), &dUpdateObject);
	hkUpdateObjectVM.createHook(reinterpret_cast<LPVOID>(m_Game->m_Offsets->UpdateObjectVM.address), &dUpdateObjectVM);
	hkRotateObject.createHook(reinterpret_cast<LPVOID>(m_Game->m_Offsets->RotateObject.address), &dRotateObject);
	hkEyeAngles.createHook(reinterpret_cast<LPVOID>(m_Game->m_Offsets->EyeAngles.address), &dEyeAngles);

	// Portal Gun VFX
	hkGetDefaultFOV.createHook(reinterpret_cast<LPVOID>(m_Game->m_Offsets->GetDefaultFOV.address), &dGetDefaultFOV);
	hkGetFOV.createHook(reinterpret_cast<LPVOID>(m_Game->m_Offsets->GetFOV.address), &dGetFOV);
	hkGetViewModelFOV.createHook(reinterpret_cast<LPVOID>(m_Game->m_Offsets->GetViewModelFOV.address), &dGetViewModelFOV);
	
	// Laser Pointer
	GetPortalPlayer = reinterpret_cast<tGetPortalPlayer>(m_Game->m_Offsets->GetPortalPlayer.address);
	CreatePingPointer = reinterpret_cast<tCreatePingPointer>(m_Game->m_Offsets->CreatePingPointer.address);
	PrecacheParticleSystem = reinterpret_cast<tPrecacheParticleSystem>(m_Game->m_Offsets->PrecacheParticleSystem.address);
	hkPrecache.createHook(reinterpret_cast<LPVOID>(m_Game->m_Offsets->Precache.address), &dPrecache);
	hkSetDrawOnlyForSplitScreenUser.createHook(reinterpret_cast<LPVOID>(m_Game->m_Offsets->SetDrawOnlyForSplitScreenUser.address), &dSetDrawOnlyForSplitScreenUser);
	hkCHudCrosshair_ShouldDraw.createHook(reinterpret_cast<LPVOID>(m_Game->m_Offsets->CHudCrosshair_ShouldDraw.address), &dCHudCrosshair_ShouldDraw);

	//
	EntityIndex = reinterpret_cast<tEntindex>(m_Game->m_Offsets->CBaseEntity_entindex.address);
	GetOwner = reinterpret_cast<tGetOwner>(m_Game->m_Offsets->GetOwner.address);
	return 1;
} 

bool __fastcall Hooks::dCHudCrosshair_ShouldDraw(void* ecx, void* edx) {
	bool shouldDraw = hkCHudCrosshair_ShouldDraw.fOriginal(ecx);

	m_VR->m_DrawCrosshair = shouldDraw;

	return ((m_VR->m_AimMode == 1) ? shouldDraw : false);
}

void __fastcall Hooks::dPrecache(void* ecx, void* edx) {
	hkPrecache.fOriginal(ecx);
	PrecacheParticleSystem("robot_point_beam");
}

void __fastcall Hooks::dSetDrawOnlyForSplitScreenUser(void* ecx, void* edx, int nSlot) {
	hkSetDrawOnlyForSplitScreenUser.fOriginal(ecx, -1);
}

void __fastcall Hooks::dRenderView(void *ecx, void *edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw)
{
	if (!m_VR->m_CreatedVRTextures) {
		m_VR->CreateVRTextures();
	}

	if (m_Game->m_VguiSurface->IsCursorVisible())
		return hkRenderView.fOriginal(ecx, setup, hudViewSetup, nClearFlags, whatToDraw);

	IMaterialSystem* matSystem = m_Game->m_MaterialSystem;

	hudViewSetup.width = m_VR->m_RenderWidth;
	hudViewSetup.height = m_VR->m_RenderHeight;
	hudViewSetup.fov = m_VR->m_Fov;
	hudViewSetup.m_flAspectRatio = m_VR->m_Aspect;

	hudViewSetup.m_nUnscaledWidth = m_VR->m_RenderWidth;
	hudViewSetup.m_nUnscaledHeight = m_VR->m_RenderHeight;

	Vector position = setup.origin;

	if (m_VR->m_ApplyPortalRotationOffset) {
		Vector vec = position - m_VR->m_SetupOrigin;
		float distance = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);

		// Rudimentary portalling detection
		if (distance > 35) {
			//m_VR->m_RotationOffset.x += m_VR->m_PortalRotationOffset.x;
			m_VR->m_RotationOffset.y += m_VR->m_PortalRotationOffset.y;
			//m_VR->m_RotationOffset.z += m_VR->m_PortalRotationOffset.z;

			m_VR->UpdateHMDAngles();

			m_VR->m_ApplyPortalRotationOffset = false;
		}
	}

	m_VR->m_SetupOrigin = position;

	Vector hmdAngle = m_VR->GetViewAngle();
	QAngle inGameAngle(hmdAngle.x, hmdAngle.y, hmdAngle.z);
	m_Game->m_EngineClient->SetViewAngles(inGameAngle);

	float aspect = setup.m_flAspectRatio;

	setup.x = 0;
	setup.y = 0;
	setup.width = m_VR->m_RenderWidth;
	setup.height = m_VR->m_RenderHeight;
	setup.m_nUnscaledWidth = m_VR->m_RenderWidth;
	setup.m_nUnscaledHeight = m_VR->m_RenderHeight;
	setup.fov = m_VR->m_Fov;
	setup.fovViewmodel = m_VR->m_Fov;
	setup.m_flAspectRatio = m_VR->m_Aspect;
	setup.zNear = 6;
	setup.zNearViewmodel = 2;
	setup.angles = hmdAngle;

	CViewSetup leftEyeView = setup;
	CViewSetup rightEyeView = setup;

	int playerIndex = m_Game->m_EngineClient->GetLocalPlayer();
	auto* localPlayer = reinterpret_cast<C_BasePlayer*>(m_Game->GetClientEntity(playerIndex));

	// Left eye CViewSetup
	QAngle tempAngle = QAngle(setup.angles.x, setup.angles.y, setup.angles.z);
	leftEyeView.origin = m_VR->TraceEye(reinterpret_cast<uint32_t*>(localPlayer), position, m_VR->GetViewOriginLeft(position), tempAngle);
	leftEyeView.angles.y = tempAngle.y;

	IMatRenderContext* rndrContext = matSystem->GetRenderContext();
	rndrContext->SetRenderTarget(m_VR->m_LeftEyeTexture);
	hkRenderView.fOriginal(ecx, leftEyeView, hudViewSetup, nClearFlags, whatToDraw);
	
	// Right eye CViewSetup
	tempAngle = QAngle(setup.angles.x, setup.angles.y, setup.angles.z);
	rightEyeView.origin = m_VR->TraceEye(reinterpret_cast<uint32_t*>(localPlayer), position, m_VR->GetViewOriginRight(position), tempAngle);
	rightEyeView.angles.y = tempAngle.y;

	rndrContext->SetRenderTarget(m_VR->m_RightEyeTexture);
	hkRenderView.fOriginal(ecx, rightEyeView, hudViewSetup, nClearFlags, whatToDraw);

	m_PushedHud = false;

	rndrContext->SetRenderTarget(NULL);
	rndrContext->Release();

	if (m_VR->m_RenderWindow) {
		setup.m_flAspectRatio = aspect;

		//setup.width, setup.height
		hkRenderView.fOriginal(ecx, setup, hudViewSetup, nClearFlags, whatToDraw);
	}


	m_VR->m_RenderedNewFrame = true;
}

bool __fastcall Hooks::dCreateMove(void *ecx, void *edx, float flInputSampleTime, CUserCmd *cmd)
{
	if (!cmd->command_number)
		return hkCreateMove.fOriginal(ecx, flInputSampleTime, cmd);

	if (m_VR->m_IsVREnabled)
	{
		cmd->viewangles = m_VR->m_HmdAngAbs;

		// Roomscale and joystick walk are mutually exclusive:
		// physical movement takes priority, joystick is used only when standing still.

		bool hasRoomscaleMovement = false;

		if (m_VR->m_RoomscaleActive)
		{
			Vector setupOriginToHMD = (m_VR->m_HmdPosRelativeRaw - m_VR->m_HmdPosRelativeRawPrev) * m_VR->m_VRScale;
			m_VR->m_HmdPosRelativeRawPrev = m_VR->m_HmdPosRelativeRaw;

			setupOriginToHMD.z = 0;
			if (VectorLength(setupOriginToHMD) > 1.0f)
			{
				hasRoomscaleMovement = true;
				cmd->forwardmove = 0;
				cmd->sidemove = 0;
				cmd->buttons &= ~(IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT);

				float forwardSpeed = DotProduct2D(setupOriginToHMD, m_VR->m_HmdForward);
				float sideSpeed = DotProduct2D(setupOriginToHMD, m_VR->m_HmdRight);
				cmd->forwardmove += forwardSpeed;
				cmd->sidemove += sideSpeed;

				if (cmd->forwardmove > 0.0f)
					cmd->buttons |= IN_FORWARD;
				else if (cmd->forwardmove < 0.0f)
					cmd->buttons |= IN_BACK;

				if (cmd->sidemove > 0.0f)
					cmd->buttons |= IN_MOVELEFT;
				else if (cmd->sidemove < 0.0f)
					cmd->buttons |= IN_MOVERIGHT;
			}
		}

		if (!hasRoomscaleMovement)
		{
			vr::InputAnalogActionData_t analogActionData;
			if (m_VR->GetAnalogActionData(m_VR->m_ActionWalk, analogActionData))
			{
				cmd->buttons &= ~(IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT);

				cmd->forwardmove += analogActionData.y * MAX_LINEAR_SPEED;
				cmd->sidemove += analogActionData.x * MAX_LINEAR_SPEED;
				cmd->upmove = 0.0f;

				if (cmd->forwardmove > 0.0f)
					cmd->buttons |= IN_FORWARD;
				else if (cmd->forwardmove < 0.0f)
					cmd->buttons |= IN_BACK;

				if (cmd->sidemove > 0.0f)
					cmd->buttons |= IN_MOVELEFT;
				else if (cmd->sidemove < 0.0f)
					cmd->buttons |= IN_MOVERIGHT;
			}
		}
	}

	return false;
}

void __fastcall Hooks::dCalcViewModelView(void *ecx, void *edx, const Vector &eyePosition, const QAngle &eyeAngles)
{
	Vector vecNewOrigin = eyePosition;
	QAngle vecNewAngles = eyeAngles;

	//std::cout << "dCalcViewModelView: (" << m_VR->m_IsVREnabled << ")\n";

	if (m_VR->m_IsVREnabled)
	{
		vecNewOrigin = m_VR->GetRecommendedViewmodelAbsPos(eyePosition);
		vecNewAngles = m_VR->GetRecommendedViewmodelAbsAngle();
	}


	return hkCalcViewModelView.fOriginal(ecx, vecNewOrigin, vecNewAngles);
}

float __fastcall Hooks::dProcessUsercmds(void *ecx, void *edx, edict_t *player, void *buf, int numcmds, int totalcmds, int dropped_packets, bool ignore, bool paused)
{
	if (player && player->m_pUnk)
	{
		Server_BaseEntity *pPlayer = reinterpret_cast<Server_BaseEntity*>(player->m_pUnk->GetBaseEntity());
		int index = EntityIndex(pPlayer);
		if (index >= 0 && index < static_cast<int>(m_Game->m_PlayersVRInfo.size()))
			m_Game->m_CurrentUsercmdID = index;
	}

	auto result = hkProcessUsercmds.fOriginal(ecx, player, buf, numcmds, totalcmds, dropped_packets, ignore, paused);
	m_Game->m_CurrentUsercmdID = -1;
	return result;
}

int Hooks::dWriteUsercmd(bf_write *buf, CUserCmd *to, CUserCmd *from)
{
	auto result = hkWriteUsercmd.fOriginal(buf, to, from);

	// Write our stuff into the buffer after the standard command
	if (m_VR->m_IsVREnabled)
	{
		Vector rightPos = m_VR->GetRightControllerAbsPos();
		QAngle rightAng = m_VR->GetRightControllerAbsAngle();

		buf->WriteChar(-2); // marker
		buf->WriteChar(1);  // version (1 = both controllers)
		buf->WriteBitVec3Coord(rightPos);
		buf->WriteBitAngles(rightAng);
		buf->WriteBitVec3Coord(m_VR->GetLeftControllerAbsPos());
		buf->WriteBitAngles(m_VR->m_LeftControllerAngAbs);
	}

	return result;
}

int Hooks::dReadUsercmd(bf_read *buf, CUserCmd* move, CUserCmd* from)
{
	auto result = hkReadUsercmd.fOriginal(buf, move, from);

	int i = m_Game->m_CurrentUsercmdID;
	if (i < 0 || i >= static_cast<int>(m_Game->m_PlayersVRInfo.size()))
		return result;

	auto& vrPlayer = m_Game->m_PlayersVRInfo[i];

	auto pos = buf->Tell();
	int res = buf->ReadChar();

	// This means we got a VR player on the other side
	// -2 marker followed by version byte + controller data
	if (res == -2 && buf->GetNumBitsLeft() >= 80)
	{
		vrPlayer.isUsingVR = true;
		int version = buf->ReadChar();

		buf->ReadBitVec3Coord(vrPlayer.controllerPos);
		buf->ReadBitAngles(vrPlayer.controllerAngle);

		if (version >= 1)
		{
			buf->ReadBitVec3Coord(vrPlayer.leftControllerPos);
			buf->ReadBitAngles(vrPlayer.leftControllerAngle);
		}
	}
	else
	{
		buf->Seek(pos);
	}

	return result;
}


int Hooks::dGetPrimaryAttackActivity(void *ecx, void *edx, void *meleeInfo)
{
	return hkGetPrimaryAttackActivity.fOriginal(ecx, meleeInfo);
}

Vector *Hooks::dEyePosition(void *ecx, void *edx, Vector *eyePos)
{
	Vector *result = hkEyePosition.fOriginal(ecx, eyePos);
	return result;
}

void Hooks::dDrawModelExecute(void *ecx, void *edx, void *state, const ModelRenderInfo_t &info, void *pCustomBoneToWorld)
{
	if (info.pModel && !m_Game->m_CachedArmsModel)
	{
		const char* modelName = m_Game->m_ModelInfo->GetModelName(info.pModel);
		if (strstr(modelName, "/arms/"))
		{
			m_Game->m_ArmsMaterial = m_Game->m_MaterialSystem->FindMaterial(modelName, "Model textures");
			m_Game->m_ArmsModel = info.pModel;
			m_Game->m_CachedArmsModel = true;
		}
	}

	if (info.pModel && info.pModel == m_Game->m_ArmsModel)
	{
		m_Game->m_ArmsMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		m_Game->m_ModelRender->ForcedMaterialOverride(m_Game->m_ArmsMaterial);
		hkDrawModelExecute.fOriginal(ecx, state, info, pCustomBoneToWorld);
		m_Game->m_ModelRender->ForcedMaterialOverride(NULL);
		return;
	}

	hkDrawModelExecute.fOriginal(ecx, state, info, pCustomBoneToWorld);
}

void Hooks::dPushRenderTargetAndViewport(void *ecx, void *edx, ITexture *pTexture, ITexture *pDepthTexture, int nViewX, int nViewY, int nViewW, int nViewH)
{
	if (m_VR->m_CreatedVRTextures && !m_PushedHud)
	{
		pTexture = m_VR->m_HUDTexture;

		//pTexture = m_VR->m_RightEyeTexture;

		IMatRenderContext *renderContext = m_Game->m_MaterialSystem->GetRenderContext();
		renderContext->ClearBuffers(false, true, true);
		renderContext->Release();

		hkPushRenderTargetAndViewport.fOriginal(ecx, pTexture, pDepthTexture, nViewX, nViewY, nViewW, nViewH);

		renderContext = m_Game->m_MaterialSystem->GetRenderContext();
		renderContext->OverrideAlphaWriteEnable(true, true);
		renderContext->ClearColor4ub(0, 0, 0, 0);
		renderContext->ClearBuffers(true, false);
		renderContext->Release();

		m_VR->m_RenderedHud = true;
		m_PushedHud = true;
	}
	else
	{
		hkPushRenderTargetAndViewport.fOriginal(ecx, pTexture, pDepthTexture, nViewX, nViewY, nViewW, nViewH);
	}
}

void Hooks::dPopRenderTargetAndViewport(void *ecx, void *edx)
{
	if (!m_VR->m_CreatedVRTextures)
		return hkPopRenderTargetAndViewport.fOriginal(ecx);

	//std::cout << "dPopRenderTargetAndViewport: " << m_PushHUDStep << "\n";

	m_PushHUDStep = 0;

	if (m_PushedHud)
	{
		IMatRenderContext* renderContext = m_Game->m_MaterialSystem->GetRenderContext();
		renderContext->OverrideAlphaWriteEnable(false, true);
		renderContext->ClearColor4ub(0, 0, 0, 255);
		renderContext->Release();
	}

	hkPopRenderTargetAndViewport.fOriginal(ecx);
}

void Hooks::dVGui_Paint(void *ecx, void *edx, int mode)
{
	if (!m_VR->m_CreatedVRTextures || m_VR->m_Game->m_VguiSurface->IsCursorVisible())
		return hkVgui_Paint.fOriginal(ecx, mode);

	//std::cout << "dVGui_Paint\n";

	if (m_PushedHud)
		mode = PAINT_UIPANELS | PAINT_INGAMEPANELS;

	hkVgui_Paint.fOriginal(ecx, mode);
}

int Hooks::dIsSplitScreen()
{
	//std::cout << "dIsSplitScreen: " << m_PushHUDStep << "\n";

	if (m_PushHUDStep == 0)
		++m_PushHUDStep;
	else
		m_PushHUDStep = -999;

	return hkIsSplitScreen.fOriginal();
}

DWORD *Hooks::dPrePushRenderTarget(void *ecx, void *edx, int a2)
{
	//std::cout << "dPrePushRenderTarget: " << m_PushHUDStep << "\n";

	if (m_PushHUDStep == 1)
		++m_PushHUDStep;
	else
		m_PushHUDStep = -999;

	return hkPrePushRenderTarget.fOriginal(ecx, a2);
}

Vector* Hooks::dWeapon_ShootPosition(void* ecx, void* edx, Vector* eyePos)
{
	Vector* result = hkWeapon_ShootPosition.fOriginal(ecx, eyePos);

	int localIndex = m_Game->m_EngineClient->GetLocalPlayer();
	int index = EntityIndex(ecx);

	if (m_VR->m_IsVREnabled && localIndex == index) {
		*result = m_VR->GetRightControllerAbsPos();	
	}
	else if (index >= 0 && index < static_cast<int>(m_Game->m_PlayersVRInfo.size()))
	{
		auto& vrPlayer = m_Game->m_PlayersVRInfo[index];
		if (vrPlayer.isUsingVR)
			*result = vrPlayer.controllerPos;
	}

	return result;
}

bool __fastcall Hooks::dTraceFirePortal(void* ecx, void* edx, const Vector& vTraceStart, const Vector& vDirection, bool bPortal2, int iPlacedBy, void* tr) //trace_tx& tr, Vector& vFinalPosition //  , Vector& vFinalPosition, QAngle& qFinalAngles, int iPlacedBy, bool bTest /*= false*/
{
	Vector vNewTraceStart = vTraceStart;
	Vector vNewDirection = vDirection;

	if (iPlacedBy == 2) {
		int localIndex = m_Game->m_EngineClient->GetLocalPlayer();

		auto owner = GetOwner(ecx);

		if (owner) {
			int index = EntityIndex(owner);

			if (m_VR->m_IsVREnabled && localIndex == index) {
				vNewTraceStart = m_VR->GetRightControllerAbsPos();
				vNewDirection = m_VR->m_RightControllerForward;
			}
			else if (index >= 0 && index < static_cast<int>(m_Game->m_PlayersVRInfo.size()))
			{
				auto& vrPlayer = m_Game->m_PlayersVRInfo[index];
				if (vrPlayer.isUsingVR)
				{
					vNewTraceStart = vrPlayer.controllerPos;
					Vector fwd, rt, up;
					QAngle::AngleVectors(vrPlayer.controllerAngle, &fwd, &rt, &up);
					vNewDirection = fwd;
				}
			}
		}
	}

	return hkTraceFirePortal.fOriginal(ecx, vNewTraceStart, vNewDirection, bPortal2, iPlacedBy, tr);
}

void __fastcall Hooks::dPlayerPortalled(void* ecx, void* edx, void* a2, __int64 a3)
{
	CBaseEntity* pBaseEntity = reinterpret_cast<CBaseEntity*>(ecx);

	QAngle angAbsRotationBefore;
	m_Game->m_EngineClient->GetViewAngles(angAbsRotationBefore);

	hkPlayerPortalled.fOriginal(ecx, a2, a3);

	QAngle angAbsRotationAfter;
	m_Game->m_EngineClient->GetViewAngles(angAbsRotationAfter);

	if (angAbsRotationBefore != angAbsRotationAfter) {
		m_VR->m_PortalRotationOffset = angAbsRotationAfter - angAbsRotationBefore;
		m_VR->m_ApplyPortalRotationOffset = true;
	}

	return;
}

int Hooks::dGetModeHeight(void* ecx, void* edx) {
	//std::cout << "dGetModeHeight\n";
	return m_VR->m_RenderHeight;
}

bool Hooks::dClipTransform(const Vector& point, Vector* pScreen)
{
	return hkClipTransform.fOriginal(point, pScreen);
}

bool Hooks::ScreenTransform(const Vector& point, Vector* pScreen, int width, int height)
{
	bool retval = hkClipTransform.fOriginal(point, pScreen);

	pScreen->x = 0.5f * (pScreen->x + 1.0f) * width;
	pScreen->y = 0.5f * (-pScreen->y + 1.0f) * height;

	return retval;
}

int __fastcall Hooks::dDrawSelf(void* ecx, void* edx, int x, int y, int w, int h, const void* clr, float flApparentZ) {
	//std::cout << "dDrawSelf - X: " << x << ", Y: " << y << ", W: " << w << ", H: " << h << ", Z: " << flApparentZ << "\n";

	//int playerIndex = m_Game->m_EngineClient->GetLocalPlayer();

	//auto viewport = m_Game->m_ClientMode->GetViewport();

	int newX = x;
	int	newY = y;

	if (m_VR->m_IsVREnabled)
	{
		int windowWidth, windowHeight;
		auto* ctx = m_Game->m_MaterialSystem->GetRenderContext();
		ctx->GetWindowSize(windowWidth, windowHeight);

		Vector screen = { 0, 0, 0 };

		//Vector vec = m_VR->m_AimPos - m_VR->GetRightControllerAbsPos();

		//newZ = 1.0 / sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);

		ScreenTransform(m_VR->m_AimPos, &screen, m_VR->m_RenderWidth, m_VR->m_RenderHeight);

		int offsetX = x - (windowWidth * 0.5f);
		int offsetY = y - (windowHeight * 0.5f);

		newX = screen.x + offsetX;
		newY = screen.y + offsetY;

		ctx->Release();
	}

	return hkDrawSelf.fOriginal(ecx, newX, newY, w, h, clr, flApparentZ);
}

void __cdecl Hooks::dVGui_GetHudBounds(int slot, int& x, int& y, int& w, int& h) {
	if (m_VR->m_IsVREnabled && !m_Game->m_VguiSurface->IsCursorVisible())
	{
		x = y = 0;
		w = m_VR->m_RenderWidth;
		h = m_VR->m_RenderHeight;
	} else {
		hkVGui_GetHudBounds.fOriginal(slot, x, y, w, h);
	}

	//std::cout << "dVGui_GetHudBounds - X: " << x << ", Y: " << y << ", W: " << w << ", H: " << h << "\n";
}

void __cdecl Hooks::dVGui_GetPanelBounds(int slot, int& x, int& y, int& w, int& h) {
	if (m_VR->m_IsVREnabled && !m_Game->m_VguiSurface->IsCursorVisible())
	{
		x = y = 0;
		w = m_VR->m_RenderWidth;
		h = m_VR->m_RenderHeight;
	}
	else {
		hkVGui_GetPanelBounds.fOriginal(slot, x, y, w, h);
	}

	//std::cout << "dVGui_GetPanelBounds - X: " << x << ", Y: " << y << ", W: " << w << ", H: " << h << "\n";
}

void __cdecl Hooks::dVGUI_UpdateScreenSpaceBounds(int nNumSplits, int sx, int sy, int sw, int sh) {
	hkVGUI_UpdateScreenSpaceBounds.fOriginal(nNumSplits, sx, sy, m_VR->m_RenderWidth, m_VR->m_RenderHeight);
}

void __cdecl Hooks::dVGui_GetTrueScreenSize(int &w, int &h) {
	w = m_VR->m_RenderWidth;
	h = m_VR->m_RenderHeight;
}

void __fastcall Hooks::dGetScreenSize(void* ecx, void* edx, int& wide, int& tall) {
	hkGetScreenSize.fOriginal(ecx, wide, tall);
	wide = m_VR->m_RenderWidth;
	tall = m_VR->m_RenderHeight;
}

void __cdecl Hooks::dGetHudSize(int& w, int& h) {
	w = m_VR->m_RenderWidth;
	h = m_VR->m_RenderHeight;
}

void __fastcall Hooks::dPush2DView(void* ecx, void* edx, IMatRenderContext* pRenderContext, const CViewSetup& view, int nFlags, ITexture* pRenderTarget, void* frustumPlanes) {
	m_PushedHud = false;

	return hkPush2DView.fOriginal(ecx, pRenderContext, view, nFlags, pRenderTarget, frustumPlanes);
}

void __fastcall Hooks::dRender(void* ecx, void* edx, vrect_t* rect) {
	//std::cout << "dRender - X: " << rect->x << ", Y: " << rect->y << ", W: " << rect->width << ", H: " << rect->height  << "\n";

	return hkRender.fOriginal(ecx, rect);
}

void __fastcall Hooks::dSetBounds(void* ecx, void* edx, int x, int y, int w, int h) {
	hkSetBounds.fOriginal(ecx, x, y, m_VR->m_RenderWidth, m_VR->m_RenderHeight);
}

void __fastcall Hooks::dSetSize(void* ecx, void* edx, int wide, int tall) {
	hkSetSize.fOriginal(ecx, wide, tall);

	//std::cout << "dSetSize - Wide: " << wide << ", Tall: " << tall  << "\n";
}

void __fastcall Hooks::dGetClipRect(void* ecx, void* edx, int& x0, int& y0, int& x1, int& y1) {
	hkGetClipRect.fOriginal(ecx, x0, y0, x1, y1);

	//std::cout << "dGetClipRect - X: " << x0 << ", Y: " << y0 << ", W: " << x1 << ", H: " << y1  << "\n";
}

double __fastcall Hooks::dComputeError(void* ecx, void* edx) {
	bool wasTrue = m_VR->m_OverrideEyeAngles;

	m_VR->m_OverrideEyeAngles = true;

	double computedError = hkComputeError.fOriginal(edx);

	if (!wasTrue)
		m_VR->m_OverrideEyeAngles = false;

	return computedError;
}

bool __fastcall Hooks::dUpdateObject(void* ecx, void* edx, void* pPlayer, float flError, bool bIsTeleport) {
	bool wasTrue = m_VR->m_OverrideEyeAngles;

	m_VR->m_OverrideEyeAngles = true;

	bool value = hkUpdateObject.fOriginal(ecx, pPlayer, flError, bIsTeleport);

	if (!wasTrue)
		m_VR->m_OverrideEyeAngles = false;

	return value;
}

bool __fastcall Hooks::dUpdateObjectVM(void* ecx, void* edx, void* pPlayer, float flError) {
	bool wasTrue = m_VR->m_OverrideEyeAngles;

	m_VR->m_OverrideEyeAngles = true;

	bool value = hkUpdateObjectVM.fOriginal(ecx, pPlayer, flError);

	if (!wasTrue)
		m_VR->m_OverrideEyeAngles = false;

	return value;
}

void __fastcall Hooks::dRotateObject(void* ecx, void* edx, void* pPlayer, float fRotAboutUp, float fRotAboutRight, bool bUseWorldUpInsteadOfPlayerUp) {
	bool wasTrue = m_VR->m_OverrideEyeAngles;

	m_VR->m_OverrideEyeAngles = true;

	hkRotateObject.fOriginal(ecx, pPlayer, fRotAboutUp, fRotAboutRight, bUseWorldUpInsteadOfPlayerUp);

	if (!wasTrue)
		m_VR->m_OverrideEyeAngles = false;
}

QAngle& __fastcall Hooks::dEyeAngles(void* ecx, void* edx) {
	if (m_VR->m_OverrideEyeAngles) {
		int localIndex = m_Game->m_EngineClient->GetLocalPlayer();
		int index = EntityIndex(ecx);

		if (m_VR->m_IsVREnabled && localIndex == index) {
			return m_VR->GetRightControllerAbsAngleConst();
		}
		else if (index >= 0 && index < static_cast<int>(m_Game->m_PlayersVRInfo.size()))
		{
			auto& vrPlayer = m_Game->m_PlayersVRInfo[index];
			if (vrPlayer.isUsingVR)
				return vrPlayer.controllerAngle;
		}
	}

	return hkEyeAngles.fOriginal(ecx);
}

int __fastcall Hooks::dGetDefaultFOV(void* ecx, void* edx) {
	return m_VR->m_Fov;
}

double __fastcall Hooks::dGetFOV(void* ecx, void* edx) {
	return m_VR->m_Fov;
}

double __fastcall Hooks::dGetViewModelFOV(void* ecx, void* edx) {
	return m_VR->m_Fov;
}

void __cdecl Hooks::dS_UpdateListener(const Vector& origin, const Vector& forward, const Vector& right, const Vector& up)
{
	if (m_VR->m_IsVREnabled)
	{
		Vector hmdOrigin = m_VR->GetViewOrigin(m_VR->m_SetupOrigin);
		Vector hmdAngVec = m_VR->GetViewAngle();
		QAngle hmdAng(hmdAngVec.x, hmdAngVec.y, hmdAngVec.z);
		Vector hmdForward, hmdRight, hmdUp;
		QAngle::AngleVectors(hmdAng, &hmdForward, &hmdRight, &hmdUp);

		return hkS_UpdateListener.fOriginal(hmdOrigin, hmdForward, hmdRight, hmdUp);
	}

	return hkS_UpdateListener.fOriginal(origin, forward, right, up);
}

void __fastcall Hooks::dEmitSound(void* ecx, void* edx, void* filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char* pSample, float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundTime, int nSpeakerEntity, int nStartDelay)
{
	if (m_VR->m_IsVREnabled && iEntIndex >= 0)
	{
		int localIndex = m_Game->m_EngineClient->GetLocalPlayer();

		// Player-entity sounds → HMD position
		if (iEntIndex == localIndex)
		{
			Vector hmdOrigin = m_VR->GetViewOrigin(m_VR->m_SetupOrigin);
			return hkEmitSound.fOriginal(ecx, filter, iEntIndex, iChannel, pSoundEntry,
				nSoundEntryHash, pSample, flVolume, flAttenuation, nSeed, iFlags, iPitch,
				&hmdOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundTime,
				nSpeakerEntity, nStartDelay);
		}

		// Weapon sounds (CHAN_WEAPON) from client-predicted firing come from
		// the weapon entity index (not the player index).  Since we cannot
		// reliably determine ownership client-side, we redirect ALL CHAN_WEAPON
		// to the right controller.  This fixes local weapon spatialisation;
		// remote players' weapons will also use the local controller, which is
		// a minor trade-off acceptable for close-quarters coop.
		if (iChannel == CHAN_WEAPON)
		{
			Vector controllerPos = m_VR->GetRightControllerAbsPos();
			return hkEmitSound.fOriginal(ecx, filter, iEntIndex, iChannel, pSoundEntry,
				nSoundEntryHash, pSample, flVolume, flAttenuation, nSeed, iFlags, iPitch,
				&controllerPos, pDirection, pUtlVecOrigins, bUpdatePositions, soundTime,
				nSpeakerEntity, nStartDelay);
		}
	}

	return hkEmitSound.fOriginal(ecx, filter, iEntIndex, iChannel, pSoundEntry,
		nSoundEntryHash, pSample, flVolume, flAttenuation, nSeed, iFlags, iPitch,
		pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundTime,
		nSpeakerEntity, nStartDelay);
}