 //------------------------------------------------------- ----------------------
// File: Object.h
//-----------------------------------------------------------------------------

#pragma once

#include "Mesh.h"
#include "Camera.h"

#define DIR_FORWARD					0x01
#define DIR_BACKWARD				0x02
#define DIR_LEFT					0x04
#define DIR_RIGHT					0x08
#define DIR_UP						0x10
#define DIR_DOWN					0x20

class CShader;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

struct MATERIALLOADINFO
{
	XMFLOAT4						m_xmf4AlbedoColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4						m_xmf4EmissiveColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4						m_xmf4SpecularColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	float							m_fGlossiness = 0.0f;
	float							m_fSmoothness = 0.0f;
	float							m_fSpecularHighlight = 0.0f;
	float							m_fMetallic = 0.0f;
	float							m_fGlossyReflection = 0.0f;

	UINT							m_nType = 0x00;

	//char							m_pstrAlbedoMapName[64] = { '\0' };
	//char							m_pstrSpecularMapName[64] = { '\0' };
	//char							m_pstrMetallicMapName[64] = { '\0' };
	//char							m_pstrNormalMapName[64] = { '\0' };
	//char							m_pstrEmissionMapName[64] = { '\0' };
	//char							m_pstrDetailAlbedoMapName[64] = { '\0' };
	//char							m_pstrDetailNormalMapName[64] = { '\0' };
};

struct MATERIALSLOADINFO
{
	int								m_nMaterials = 0;
	MATERIALLOADINFO				*m_pMaterials = NULL;
};

class CMaterialColors
{
public:
	CMaterialColors() { }
	CMaterialColors(MATERIALLOADINFO *pMaterialInfo);
	virtual ~CMaterialColors() { }

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	XMFLOAT4						m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	XMFLOAT4						m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4						m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); //(r,g,b,a=power)
	XMFLOAT4						m_xmf4Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
};

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	CShader							*m_pShader = NULL;

	CMaterialColors					*m_pMaterialColors = NULL;

	void SetMaterialColors(CMaterialColors *pMaterialColors);
	void SetShader(CShader *pShader);
	void SetIlluminatedShader() { SetShader(m_pIlluminatedShader); }

	void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList);

protected:
	static CShader					*m_pIlluminatedShader;

public:
	static void CMaterial::PrepareShaders(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
};

class CGameObject
{
private:
	int								m_nReferences = 0;

public:
	void AddRef();
	void Release();

public:
	CGameObject();
    virtual ~CGameObject();

public:
	char							m_pstrFrameName[64];

	CMesh							*m_pMesh = NULL;

	int								m_nMaterials = 0;
	CMaterial						**m_ppMaterials = NULL;

	XMFLOAT4X4						m_xmf4x4Transform;
	XMFLOAT4X4						m_xmf4x4World;

	CGameObject 					*m_pParent = NULL;
	CGameObject 					*m_pChild = NULL;
	CGameObject 					*m_pSibling = NULL;

	BoundingOrientedBox				m_xmOOBB;

	bool							m_bIsCollisionsPossible = false;
	bool							m_bIsItem = false;

	void SetMesh(CMesh *pMesh);
	void SetShader(CShader *pShader);
	void SetShader(int nMaterial, CShader *pShader);
	void SetMaterial(int nMaterial, CMaterial *pMaterial);

	void SetChild(CGameObject* pChild, bool bReferenceUpdate = false);

	virtual void BuildMaterials(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) { }

	virtual void OnInitialize() { }
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4 *pxmf4x4Parent=NULL);

	virtual void OnPrepareRender() { }
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, CMaterial *pMaterial);

	virtual void ReleaseUploadBuffers();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetScale(float x, float y, float z);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT4 *pxmf4Quaternion);

	bool HierarchyIntersects(CGameObject* pCollisionGameObject, bool isSecond=false);

	CGameObject *GetParent() { return(m_pParent); }
	void UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent=NULL);
	CGameObject *FindFrame(char *pstrFrameName);

	void UpdateBoundingBox();

	UINT GetMeshType() { return((m_pMesh) ? m_pMesh->GetType() : 0); }

public:
	static MATERIALSLOADINFO *LoadMaterialsInfoFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FILE *pInFile);
	static CMeshLoadInfo *LoadMeshInfoFromFile(FILE *pInFile);

	static CGameObject *LoadFrameHierarchyFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, FILE *pInFile);
	static CGameObject *LoadGeometryFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *pstrFileName);

	static void PrintFrameInfo(CGameObject *pGameObject, CGameObject *pParent);
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
    virtual ~CRotatingObject();

private:
	XMFLOAT3					m_xmf3RotationAxis;
	float						m_fRotationSpeed;

public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }

	virtual void Animate(float fTimeElapsed, XMFLOAT4X4 *pxmf4x4Parent=NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
};

class CItemObject : public CRotatingObject
{
public:
	CItemObject();
	CItemObject(int nType);
	virtual ~CItemObject();

public:
	int							m_nItemType = 0;	// 1 : apple(HP)  2 : grape(MaxMoveSpeed)  3 : cherry(FireDelay)
	bool						m_bIsActive = false;
	float						m_fUntilActive = 10000.0f;

	void SetActive(bool bIsActive) { m_bIsActive = bIsActive; }
	bool GetActive() { return m_bIsActive; }

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
};

class CRevolvingObject : public CGameObject
{
public:
	CRevolvingObject();
	virtual ~CRevolvingObject();

private:
	XMFLOAT3					m_xmf3RevolutionAxis;
	float						m_fRevolutionSpeed;

public:
	void SetRevolutionSpeed(float fRevolutionSpeed) { m_fRevolutionSpeed = fRevolutionSpeed; }
	void SetRevolutionAxis(XMFLOAT3 xmf3RevolutionAxis) { m_xmf3RevolutionAxis = xmf3RevolutionAxis; }

	virtual void Animate(float fTimeElapsed, XMFLOAT4X4 *pxmf4x4Parent=NULL);
};

class CBulletObject : public CGameObject
{
public:
	CBulletObject();
	virtual ~CBulletObject();

protected:
	float						m_fSpeed;
	float						m_fBulletEffectiveRange;
	int							m_nDamage;
	float						m_fKnockBackPower;


	XMFLOAT3					m_xmf3FirePosition;

	bool						m_bIsActive;

public:
	void Update(float fTimeElapsed);

	void SetActive(bool bIsActive) { m_bIsActive = bIsActive; }
	void SetFirePosition(XMFLOAT3 xmf3FirePosition);
	void SetDamage(int nDamage) { m_nDamage = nDamage; }
	void SetKnockBackPower(float fKnockBackPower) { m_fKnockBackPower = fKnockBackPower; }

	bool GetActive() { return m_bIsActive; }

	float GetDamage() { return m_nDamage; }
	float GetKnockBackPower(){ return m_fKnockBackPower; }
};

class CTankObject : public CGameObject
{
public:
	CTankObject();
	virtual ~CTankObject();

	void BuildBullets(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature, int nBullets = 20);

	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(const XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetPosition(const XMFLOAT3& xmf3Position){ Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z), false); }
	void SetAccelSpeedXZ(float fSpeedXZ) { m_fAccelSpeedXZ = fSpeedXZ; }
	void SetSpeedRotateY(float fSpeedRotateY) { m_fSpeedRotateY = fSpeedRotateY; }
	void SetGuaranteeMaxVel(float fBounus) { m_fGuaranteeMaxVelocityXZ = fBounus; }
	void SetGuaranteeFireDelay(float fBonus) { m_fGuaranteeFireDelayTime = fBonus; }

	void SetDead(bool isDead) { m_bIsDead = isDead; }
	bool GetDead() { return m_bIsDead; }

	void SetHP(int nHP) { m_nHP = nHP; }
	int GetHP() { return m_nHP; }

	int m_nScore = 0;
	int m_nBulletDamage = 7;

	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }

	XMFLOAT3& GetVelocity() { return(m_xmf3Velocity); }
	float GetYaw() const { return(m_fYaw); }
	float GetPitch() const { return(m_fPitch); }
	float GetRoll() const { return(m_fRoll); }
	float GetAccelSpeedXZ() const { return(m_fAccelSpeedXZ); }
	float GetSpeedRotateY() const { return(m_fSpeedRotateY); }
	float GetGuaranteeMaxVel() { return m_fGuaranteeMaxVelocityXZ; }
	float GetGuaranteeFireDelay() { return m_fGuaranteeFireDelayTime; }

	void SetScaleVar(float x, float y, float z) { m_fScaleX = x;  m_fScaleY = y; m_fScaleZ = z;}
	void SetKey(UINT nAccKey, UINT nDeccKey, UINT nLeftKey, UINT nRightKey, UINT nFireKey) { m_nAccelKey = nAccKey; m_nDeccelKey = nDeccKey; m_nLeftRotateKey = nLeftKey; m_nRightRotateKey = nRightKey; m_nFireKey = nFireKey; }
	
	CGameObject* m_pTankTurret = NULL;
	XMFLOAT4X4 m_xmf4x4OriginalTurretTransform;

protected:

	XMFLOAT3					m_xmf3Position;
	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;

	float           			m_fPitch;
	float           			m_fYaw;
	float           			m_fRoll;

	XMFLOAT3					m_xmf3Velocity;
	XMFLOAT3     				m_xmf3Gravity;
	float           			m_fMaxVelocityXZ;
	float						m_fGuaranteeMaxVelocityXZ = 1.0f;
	float           			m_fMaxVelocityY;
	float           			m_fFriction;

	float						m_fScaleX;
	float						m_fScaleY;
	float						m_fScaleZ;

	float						m_fAccelSpeedXZ;
	float						m_fSpeedRotateY;

	float						m_fFireWaitingTime;
	float						m_fFireDelayTime;
	float						m_fGuaranteeFireDelayTime = 1.0f;

	bool						m_bIsDead;
	int							m_nHP;

public:
	int							m_nBullets = 0;
	CBulletObject**				m_ppBullets = NULL;

public:
	UINT						m_nAccelKey;
	UINT						m_nDeccelKey;
	UINT						m_nLeftRotateKey;
	UINT						m_nRightRotateKey;
	UINT						m_nFireKey;

public:
	virtual void OnInitialize();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
	void Release();

	void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);
	void Rotate(float x, float y, float z);

	void Update(float fTimeElapsed);

	virtual void OnPrepareRender();
	virtual void ReleaseUploadBuffers();

	void FireBullet();

	bool DamagedByBullet(CBulletObject* pBulletObject);
};