#include "ParticleEmitter.h"
#include<ParticleManager.h>

ParticleEmitter::ParticleEmitter(const Vector3& position, const float lifetime, const float currentTime, const uint32_t count, const std::string& name,const Vector3& Velocity)
{
	position_ = position;//位置
	frequency = lifetime;//寿命
	frequencyTime = currentTime;//現在の寿命
	this->count = count;//count
	name_ = name;//名前
	velocity_ = Velocity; // 風の強さ
}

void ParticleEmitter::Update()
{
	if (!isAutoEmit_) return;  // 自動発生が無効ならスキップ

	frequencyTime += 1.0f / 60.0f;

	if (frequencyTime > frequency) {
		Emit();
		frequencyTime = 0.0f;
	}
}

void ParticleEmitter::Emit()
{
	//パーティクルを発生
	ParticleManager::GetInstance()->Emit(name_, position_, count, velocity_);
}