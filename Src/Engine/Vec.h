#pragma once
#include <cmath>
#include <DirectXMath.h>

// Vectorクラス
class Vec3 {

public:

	/*===== メンバ変数 =====*/

	float x_;
	float y_;
	float z_;

private:

	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMVECTOR = DirectX::XMVECTOR;

public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	Vec3() :x_(0), y_(0), z_(0) {};
	Vec3(const float& X, const float& Y, const float& Z) : x_(X), y_(Y), z_(Z) {};
	Vec3(const float& X) :x_(X), y_(X), z_(X) {};
	Vec3(const XMFLOAT3& Rhs) :x_(Rhs.x), y_(Rhs.y), z_(Rhs.z) {};
	Vec3(const XMVECTOR& Rhs) :x_(Rhs.m128_f32[0]), y_(Rhs.m128_f32[1]), z_(Rhs.m128_f32[2]) {};

	// 内積
	inline float Dot(const Vec3& Rhs) {
		return x_ * Rhs.x_ + y_ * Rhs.y_ + z_ * Rhs.z_;
	}
	// 外積
	inline Vec3 Cross(const Vec3& Rhs) {
		return Vec3(y_ * Rhs.z_ - z_ * Rhs.y_,
			z_ * Rhs.x_ - x_ * Rhs.z_,
			x_ * Rhs.y_ - y_ * Rhs.x_);
	}
	// 長さ
	inline float Length() {
		return sqrtf(Dot(*this));
	}
	// 正規化
	inline void Normalize() {
		float length = this->Length();
		x_ /= length;
		y_ /= length;
		z_ /= length;
	}
	// 正規化した値を取得
	inline Vec3 GetNormal() {
		float length = this->Length();
		Vec3 buff = *this;
		buff.x_ /= length;
		buff.y_ /= length;
		buff.z_ /= length;
		return buff;
	}
	// XMFLOAT3へ変換
	inline XMFLOAT3 ConvertXMFLOAT3() {
		return XMFLOAT3(x_, y_, z_);
	}
	// XMVECTORへ変換
	inline XMVECTOR ConvertXMVECTOR() {
		return { x_,y_,z_ };
	}


#pragma region オペレーター演算子
	Vec3 operator-() const {
		return Vec3(-x_, -y_, -z_);
	}
	Vec3 operator+(const Vec3& Rhs) const {
		return Vec3(x_ + Rhs.x_, y_ + Rhs.y_, z_ + Rhs.z_);
	};
	Vec3 operator-(const Vec3& Rhs)const {
		return Vec3(x_ - Rhs.x_, y_ - Rhs.y_, z_ - Rhs.z_);
	};
	Vec3 operator*(const Vec3& Rhs)const {
		return Vec3(x_ * Rhs.x_, y_ * Rhs.y_, z_ * Rhs.z_);
	};
	Vec3 operator*(const float& Rhs)const {
		return Vec3(x_ * Rhs, y_ * Rhs, z_ * Rhs);
	};
	Vec3 operator/(const Vec3& Rhs)const {
		return Vec3(x_ / Rhs.x_, y_ / Rhs.y_, z_ / Rhs.z_);
	};
	Vec3 operator/(const float& Rhs)const {
		return Vec3(x_ / Rhs, y_ / Rhs, z_ / Rhs);
	};
	Vec3 operator%(const Vec3& Rhs) const {
		return Vec3(fmodf(x_, Rhs.x_), fmodf(y_, Rhs.y_), fmodf(z_, Rhs.z_));
	};
	void operator=(const Vec3& Rhs) {
		x_ = Rhs.x_;
		y_ = Rhs.y_;
		z_ = Rhs.z_;
	};
	bool operator==(const Vec3& Rhs)const {
		return (x_ == Rhs.x_ && y_ == Rhs.y_ && z_ == Rhs.z_);
	};
	bool operator!=(const Vec3& Rhs)const {
		return !(*this == Rhs);
	};
	void operator+=(const Vec3& Rhs) {
		x_ += Rhs.x_;
		y_ += Rhs.y_;
		z_ += Rhs.z_;
	};
	void operator-=(const Vec3& Rhs) {
		x_ -= Rhs.x_;
		y_ -= Rhs.y_;
		z_ -= Rhs.z_;
	};
	void operator*=(const Vec3& Rhs) {
		x_ *= Rhs.x_;
		y_ *= Rhs.y_;
		z_ *= Rhs.z_;
	};
	void operator/=(const Vec3& Rhs) {
		x_ /= Rhs.x_;
		y_ /= Rhs.y_;
		z_ /= Rhs.z_;
	};
	void operator%=(const Vec3& Rhs) {
		x_ = fmodf(x_, Rhs.x_);
		y_ = fmodf(y_, Rhs.y_);
		z_ = fmodf(z_, Rhs.z_);
	};

	void operator+=(const float& Rhs) {
		x_ += Rhs;
		y_ += Rhs;
		z_ += Rhs;
	};
	void operator-=(const float& Rhs) {
		x_ -= Rhs;
		y_ -= Rhs;
		z_ -= Rhs;
	};
	void operator*=(const float& Rhs) {
		x_ *= Rhs;
		y_ *= Rhs;
		z_ *= Rhs;
	};
	void operator/=(const float& Rhs) {
		x_ /= Rhs;
		y_ /= Rhs;
		z_ /= Rhs;
	};
	void operator%=(const float& Rhs) {
		x_ = fmodf(x_, Rhs);
		y_ = fmodf(y_, Rhs);
		z_ = fmodf(z_, Rhs);
	};

	/*===== DirectXMath関係のオペレーター演算子 =====*/

	Vec3 operator+(const XMFLOAT3& Rhs) const {
		return Vec3(x_ + Rhs.x, y_ + Rhs.y, z_ + Rhs.z);
	};
	Vec3 operator-(const XMFLOAT3& Rhs)const {
		return Vec3(x_ - Rhs.x, y_ - Rhs.y, z_ - Rhs.z);
	};
	Vec3 operator*(const XMFLOAT3& Rhs)const {
		return Vec3(x_ * Rhs.x, y_ * Rhs.y, z_ * Rhs.z);
	};
	Vec3 operator/(const XMFLOAT3& Rhs)const {
		return Vec3(x_ / Rhs.x, y_ / Rhs.y, z_ / Rhs.z);
	};
	Vec3 operator%(const XMFLOAT3& Rhs) const {
		return Vec3(fmodf(x_, Rhs.x), fmodf(y_, Rhs.y), fmodf(z_, Rhs.z));
	};
	void operator=(const XMFLOAT3& Rhs) {
		x_ = Rhs.x;
		y_ = Rhs.y;
		z_ = Rhs.z;
	};
	bool operator==(const XMFLOAT3& Rhs)const {
		return (x_ == Rhs.x && y_ == Rhs.y && z_ == Rhs.z);
	};
	bool operator!=(const XMFLOAT3& Rhs)const {
		return !(*this == Rhs);
	};
	void operator+=(const XMFLOAT3& Rhs) {
		x_ += Rhs.x;
		y_ += Rhs.y;
		z_ += Rhs.z;
	};
	void operator-=(const XMFLOAT3& Rhs) {
		x_ -= Rhs.x;
		y_ -= Rhs.y;
		z_ -= Rhs.z;
	};
	void operator*=(const XMFLOAT3& Rhs) {
		x_ *= Rhs.x;
		y_ *= Rhs.y;
		z_ *= Rhs.z;
	};
	void operator/=(const XMFLOAT3& Rhs) {
		x_ /= Rhs.x;
		y_ /= Rhs.y;
		z_ /= Rhs.z;
	};
	void operator%=(const XMFLOAT3& Rhs) {
		x_ = fmodf(x_, Rhs.x);
		y_ = fmodf(y_, Rhs.y);
		z_ = fmodf(z_, Rhs.z);
	};

	Vec3 operator+(const XMVECTOR& Rhs) const {
		return Vec3(x_ + Rhs.m128_f32[0], y_ + Rhs.m128_f32[1], z_ + Rhs.m128_f32[2]);
	};
	Vec3 operator-(const XMVECTOR& Rhs)const {
		return Vec3(x_ - Rhs.m128_f32[0], y_ - Rhs.m128_f32[1], z_ - Rhs.m128_f32[2]);
	};
	Vec3 operator*(const XMVECTOR& Rhs)const {
		return Vec3(x_ * Rhs.m128_f32[0], y_ * Rhs.m128_f32[1], z_ * Rhs.m128_f32[2]);
	};
	Vec3 operator/(const XMVECTOR& Rhs)const {
		return Vec3(x_ / Rhs.m128_f32[0], y_ / Rhs.m128_f32[1], z_ / Rhs.m128_f32[2]);
	};
	Vec3 operator%(const XMVECTOR& Rhs) const {
		return Vec3(fmodf(x_, Rhs.m128_f32[0]), fmodf(y_, Rhs.m128_f32[1]), fmodf(z_, Rhs.m128_f32[2]));
	};
	void operator=(const XMVECTOR& Rhs) {
		x_ = Rhs.m128_f32[0];
		y_ = Rhs.m128_f32[1];
		z_ = Rhs.m128_f32[2];
	};
	bool operator==(const XMVECTOR& Rhs)const {
		return (x_ == Rhs.m128_f32[0] && y_ == Rhs.m128_f32[1] && z_ == Rhs.m128_f32[2]);
	};
	bool operator!=(const XMVECTOR& Rhs)const {
		return !(*this == Rhs);
	};
	void operator+=(const XMVECTOR& Rhs) {
		x_ += Rhs.m128_f32[0];
		y_ += Rhs.m128_f32[1];
		z_ += Rhs.m128_f32[2];
	};
	void operator-=(const XMVECTOR& Rhs) {
		x_ -= Rhs.m128_f32[0];
		y_ -= Rhs.m128_f32[1];
		z_ -= Rhs.m128_f32[2];
	};
	void operator*=(const XMVECTOR& Rhs) {
		x_ *= Rhs.m128_f32[0];
		y_ *= Rhs.m128_f32[1];
		z_ *= Rhs.m128_f32[2];
	};
	void operator/=(const XMVECTOR& Rhs) {
		x_ /= Rhs.m128_f32[0];
		y_ /= Rhs.m128_f32[1];
		z_ /= Rhs.m128_f32[2];
	};
	void operator%=(const XMVECTOR& Rhs) {
		x_ = fmodf(x_, Rhs.m128_f32[0]);
		y_ = fmodf(y_, Rhs.m128_f32[1]);
		z_ = fmodf(z_, Rhs.m128_f32[2]);
	};

#pragma endregion

};


class Vec2 {

public:

	/*===== メンバ変数 =====*/

	float x_;
	float y_;

private:

	using XMFLOAT2 = DirectX::XMFLOAT2;

public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	Vec2() :x_(0), y_(0) {};
	Vec2(const float& X, const float& Y) : x_(X), y_(Y) {};
	Vec2(const float& X) :x_(X), y_(X) {};
	Vec2(const XMFLOAT2& Rhs) :x_(Rhs.x), y_(Rhs.y){};

	// 内積
	inline float Dot(const Vec2& Rhs) {
		return x_ * Rhs.x_ + y_ * Rhs.y_;
	}
	// 外積
	inline Vec2 Cross(const Vec2& Rhs) {
		return x_ * Rhs.y_ - y_ * Rhs.x_;
	}
	// 長さ
	inline float Length() {
		return sqrtf(Dot(*this));
	}
	// 正規化
	inline void Normalize() {
		float length = this->Length();
		x_ /= length;
		y_ /= length;
	}
	// 正規化した値を取得
	inline Vec2 GetNormal() {
		float length = this->Length();
		Vec2 buff = *this;
		buff.x_ /= length;
		buff.y_ /= length;
		return buff;
	}
	// XMFLOAT2へ変換
	inline XMFLOAT2 ConvertXMFLOAT2() {
		return XMFLOAT2(x_, y_);
	}


#pragma region オペレーター演算子
	Vec2 operator-() const {
		return Vec2(-x_, -y_);
	}
	Vec2 operator+(const Vec2& Rhs) const {
		return Vec2(x_ + Rhs.x_, y_ + Rhs.y_);
	};
	Vec2 operator-(const Vec2& Rhs)const {
		return Vec2(x_ - Rhs.x_, y_ - Rhs.y_);
	};
	Vec2 operator*(const Vec2& Rhs)const {
		return Vec2(x_ * Rhs.x_, y_ * Rhs.y_);
	};
	Vec2 operator*(const float& Rhs)const {
		return Vec2(x_ * Rhs, y_ * Rhs);
	};
	Vec2 operator/(const Vec2& Rhs)const {
		return Vec2(x_ / Rhs.x_, y_ / Rhs.y_);
	};
	Vec2 operator/(const float& Rhs)const {
		return Vec2(x_ / Rhs, y_ / Rhs);
	};
	Vec2 operator%(const Vec2& Rhs) const {
		return Vec2(fmodf(x_, Rhs.x_), fmodf(y_, Rhs.y_));
	};
	void operator=(const Vec2& Rhs) {
		x_ = Rhs.x_;
		y_ = Rhs.y_;
	};
	bool operator==(const Vec2& Rhs)const {
		return (x_ == Rhs.x_ && y_ == Rhs.y_);
	};
	bool operator!=(const Vec2& Rhs)const {
		return !(*this == Rhs);
	};
	void operator+=(const Vec2& Rhs) {
		x_ += Rhs.x_;
		y_ += Rhs.y_;
	};
	void operator-=(const Vec2& Rhs) {
		x_ -= Rhs.x_;
		y_ -= Rhs.y_;
	};
	void operator*=(const Vec2& Rhs) {
		x_ *= Rhs.x_;
		y_ *= Rhs.y_;
	};
	void operator/=(const Vec2& Rhs) {
		x_ /= Rhs.x_;
		y_ /= Rhs.y_;
	};
	void operator%=(const Vec2& Rhs) {
		x_ = fmodf(x_, Rhs.x_);
		y_ = fmodf(y_, Rhs.y_);
	};

	void operator+=(const float& Rhs) {
		x_ += Rhs;
		y_ += Rhs;
	};
	void operator-=(const float& Rhs) {
		x_ -= Rhs;
		y_ -= Rhs;
	};
	void operator*=(const float& Rhs) {
		x_ *= Rhs;
		y_ *= Rhs;
	};
	void operator/=(const float& Rhs) {
		x_ /= Rhs;
		y_ /= Rhs;
	};
	void operator%=(const float& Rhs) {
		x_ = fmodf(x_, Rhs);
		y_ = fmodf(y_, Rhs);
	};

	/*===== DirectXMath関係のオペレーター演算子 =====*/

	Vec2 operator+(const XMFLOAT2& Rhs) const {
		return Vec2(x_ + Rhs.x, y_ + Rhs.y);
	};
	Vec2 operator-(const XMFLOAT2& Rhs)const {
		return Vec2(x_ - Rhs.x, y_ - Rhs.y);
	};
	Vec2 operator*(const XMFLOAT2& Rhs)const {
		return Vec2(x_ * Rhs.x, y_ * Rhs.y);
	};
	Vec2 operator/(const XMFLOAT2& Rhs)const {
		return Vec2(x_ / Rhs.x, y_ / Rhs.y);
	};
	Vec2 operator%(const XMFLOAT2& Rhs) const {
		return Vec2(fmodf(x_, Rhs.x), fmodf(y_, Rhs.y));
	};
	void operator=(const XMFLOAT2& Rhs) {
		x_ = Rhs.x;
		y_ = Rhs.y;
	};
	bool operator==(const XMFLOAT2& Rhs)const {
		return (x_ == Rhs.x && y_ == Rhs.y);
	};
	bool operator!=(const XMFLOAT2& Rhs)const {
		return !(*this == Rhs);
	};
	void operator+=(const XMFLOAT2& Rhs) {
		x_ += Rhs.x;
		y_ += Rhs.y;
	};
	void operator-=(const XMFLOAT2& Rhs) {
		x_ -= Rhs.x;
		y_ -= Rhs.y;
	};
	void operator*=(const XMFLOAT2& Rhs) {
		x_ *= Rhs.x;
		y_ *= Rhs.y;
	};
	void operator/=(const XMFLOAT2& Rhs) {
		x_ /= Rhs.x;
		y_ /= Rhs.y;
	};
	void operator%=(const XMFLOAT2& Rhs) {
		x_ = fmodf(x_, Rhs.x);
		y_ = fmodf(y_, Rhs.y);
	};

#pragma endregion

};