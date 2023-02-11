#pragma once

template <typename T>
class Singleton
{
private:
	static T* m_Instance;

	/*********************************************************
	* @brief	�R�s�[�֎~
	********************************************************/
	Singleton(const Singleton& other) = delete;
	Singleton& operator=(const Singleton& other) = delete;

protected:
	Singleton() {};
	~Singleton() { m_Instance = nullptr; };


private:
	/*********************************************************
	* @brief	�e���v���[�g�ł͂Ȃ������N���X�̃f�X�g���N�^��
	*			���������������
	********************************************************/
	class Garbage
	{
	public:
		~Garbage()
		{
			if (Singleton::m_Instance != nullptr)
			{
				delete Singleton::m_Instance;
				Singleton::m_Instance = nullptr;
			}
		}
	};
	Garbage garbage;

public:
	static T* Instance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = new T();
		}
		return m_Instance;
	}
};

template <typename T>
T* Singleton<T>::m_Instance = nullptr;