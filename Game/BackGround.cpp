#include "stdafx.h"
#include "BackGround.h"

BackGround::BackGround()
{
}

BackGround::~BackGround()
{
}

bool BackGround::Start()
{
	AddBox({ 0.0f, -50.0f, 0.0f }, { 4000.0f, 100.0f, 4000.0f });

	AddBox({ 0.0f, 100.0f, 800.0f }, { 400.0f, 200.0f, 100.0f });
	AddBox({ 600.0f, 75.0f, 300.0f }, { 150.0f, 150.0f, 150.0f });
	AddBox({ -600.0f, 75.0f, 300.0f }, { 150.0f, 150.0f, 150.0f });
	AddBox({ 0.0f, 150.0f, -1000.0f }, { 2000.0f, 300.0f, 50.0f });

	PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();
	return true;
}

void BackGround::AddBox(const Vector3& position, const Vector3& size)
{
	auto box = std::make_unique<StaticBox>();
	box->collider.Create(size);

	RigidBodyInitData initData;
	initData.pos = position;
	initData.collider = &box->collider;
	initData.mass = 0.0f;
	box->rigidBody.Init(initData);

	m_boxes.push_back(std::move(box));
}
