#include "pipeline.h"

const Matrix4f& Pipeline::GetWorldTrans()
{
	Matrix4f ScaleTrans, RotateTrans, TranslationTrans;

	ScaleTrans.InitScaleTransform(m_scale.x, m_scale.y, m_scale.z);
	RotateTrans.InitRotateTransform(m_rotateInfo.x, m_rotateInfo.y, m_rotateInfo.z);
	TranslationTrans.InitTranslationTransform(m_worldPos.x, m_worldPos.y, m_worldPos.z);

	m_Wtransformation = TranslationTrans * RotateTrans * ScaleTrans;
	return m_Wtransformation;
}

const Matrix4f& Pipeline::GetProjTrans()
{
	m_ProjTransformation.InitPersProjTransform(m_persProjInfo);
	return m_ProjTransformation;
}

const Matrix4f& Pipeline::GetViewTrans()
{
	Matrix4f CameraTranslationTrans, CameraRotateTrans;

	CameraTranslationTrans.InitTranslationTransform(-m_camera.Pos.x, -m_camera.Pos.y, -m_camera.Pos.z);
	CameraRotateTrans.InitCameraTransform(m_camera.Target, m_camera.Up);

	m_Vtransformation = CameraRotateTrans * CameraTranslationTrans;

	return m_Vtransformation;
}

const Matrix4f& Pipeline::GetWPTrans()
{
	Matrix4f PersProjTrans;

	GetWorldTrans();
	PersProjTrans.InitPersProjTransform(m_persProjInfo);

	m_WPtransformation = PersProjTrans * m_Wtransformation;
	return m_WPtransformation;
}

const Matrix4f& Pipeline::GetVPTrans()
{
	GetViewTrans();
	GetProjTrans();

	m_VPtransformation = m_ProjTransformation * m_Vtransformation;
	return m_VPtransformation;
}

const Matrix4f& Pipeline::GetWVPTrans()
{
	GetWorldTrans();
	GetVPTrans();

	m_WVPtransformation = m_VPtransformation * m_Wtransformation;
	return m_WVPtransformation;
}


