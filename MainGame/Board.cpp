#include "main.h"
#include "Board.h"

#include "Character.h"
#include "Piece.h"

void Board::Init()
{
#pragma region ========== camera orientation ==========
	m_CameraOrientation = new TransformObject();
	m_BoardCenter = new TransformObject();

	m_CameraOrientation->GetTransform()->SetPosition(0, -10, 5);
	m_BoardCenter->GetTransform()->SetPosition(0, 0, 0);
#pragma endregion

#pragma region ========== main map ==========
	//map design
	//9: goal
	//0: normal
	//1: good start
	//2: bad start
	int squareType[SQUARE_Z_NUM][SQUARE_X_NUM] =
	{
		{0,2,0,2,0,},
		{2,0,0,0,2,},
		{0,1,9,1,0,},
		{0,0,0,0,0,},
		{0,0,9,0,0,},
	};

	//for connect squares
	Square* squareMatrix[SQUARE_Z_NUM][SQUARE_X_NUM];

	//create all square
	{
		float deltaX = SQUARE_SIZE * 1.2f;
		float posX0 = (SQUARE_X_NUM / 2.0f * (-1.0f) + 0.5f) * deltaX;
		float deltaZ = SQUARE_SIZE * 1.2f;
		float posZ0 = (SQUARE_Z_NUM / 2.0f * (-1.0f) + 0.5f) * deltaZ;
		float posY0 = -0.1f;// +((float)(rand() % 100) / 100.0f - 0.5f) * 2 * 2.0f;
		for (int z = 0; z < SQUARE_Z_NUM; z++)
		{
			for (int x = 0; x < SQUARE_X_NUM; x++)
			{
				//create square
				Square* sq = new Square();
				sq->m_ID = x + z * SQUARE_X_NUM;
				Squares.emplace(sq->m_ID, sq);

				//save for connect
				squareMatrix[z][x] = sq;

				//create obj
				SquareObject* sqObj = GameObjectManager::Create<SquareObject>();
				sq->m_SquareObject = sqObj;
				float posX = posX0 + x * deltaX;
				float posZ = posZ0 + z * deltaZ;
				float posY = posY0 + ((float)(rand() % 100) / 100.0f - 0.5f) * 2 * 0.1f;
				sqObj->GetTransform()->SetPosition(posX, posY, posZ);

				//check is bad or good start square
				switch (squareType[z][x])
				{

				case 9://set goal
				{
					new SquareType_Goal(sq);
					sqObj->m_Polygon3D->Model = DrawManager::Models[MDLID_SQUARE_GOAL];
					sqObj->GetTransform()->SetPosition(posX, posY, posZ);
				}
				break;

				case 1://set to good start square
				{
					GoodStartSquare.push_back(sq);
					new SquareType_Normal(sq);
				}
				break;

				case 2://set to bad start square
				{
					BadStartSquare.push_back(sq);
					new SquareType_Normal(sq);
				}
				break;

				}
			}
		}
	}

	//connect square
	{
		for (int z = 0; z < SQUARE_Z_NUM; z++)
		{
			for (int x = 0; x < SQUARE_X_NUM; x++)
			{
				Square* sq = squareMatrix[z][x];
				Square* sqUp = nullptr;
				Square* sqRight = nullptr;
				Square* sqDown = nullptr;
				Square* sqLeft = nullptr;

				//axis x
				if (x > 0)
				{
					sqLeft = squareMatrix[z][x - 1];
				}
				if (x < SQUARE_X_NUM - 1)
				{
					sqRight = squareMatrix[z][x + 1];
				}

				//axis z
				if (z > 0)
				{
					sqDown = squareMatrix[z - 1][x];
				}
				if (z < SQUARE_Z_NUM - 1)
				{
					sqUp = squareMatrix[z + 1][x];
				}

				sq->m_Neighbors[DIR_UP] = sqUp;
				sq->m_Neighbors[DIR_RIGHT] = sqRight;
				sq->m_Neighbors[DIR_DOWN] = sqDown;
				sq->m_Neighbors[DIR_LEFT] = sqLeft;
			}
		}
	}
#pragma endregion
	
#pragma region ========== prison room ==========
	//prison room for caught piece or dead piece

	//value for set square pos
	int idCount = SQUARE_Z_NUM * SQUARE_X_NUM;
	int max = 6;
	float deltaX = SQUARE_SIZE * 1.5f;
	float posX0 = -deltaX * max / 2.0f + deltaX * 0.5f;
	float posY0 = -0.1f;
	float posZ0 = deltaX * 3.0f;

	for (int x = 0; x < max; x++)
	{
		//create square
		Square* sq = new Square();

		//set id
		sq->m_ID = idCount + x;
		Squares.emplace(sq->m_ID, sq);

		//create and set obj
		SquareObject* sqObj = GameObjectManager::Create<SquareObject>(); 
		sqObj->m_Polygon3D->Model = DrawManager::Models[MDLID_SQUARE_PRISON];
		sq->m_SquareObject = sqObj;
		float posX = posX0 + x * deltaX;
		float posZ = posZ0;
		float posY = posY0 + ((float)(rand() % 100) / 100.0f - 0.5f) * 2 * 0.1f;
		sqObj->GetTransform()->SetPosition(posX, posY, posZ);

		//check is bad or good start square
		PrisonRoomSquare.emplace_back(sq);
		new SquareType_Normal(sq);
	}
#pragma endregion
}

void Board::Uninit()
{
	BadStartSquare.clear();
	GoodStartSquare.clear();
	PrisonRoomSquare.clear();
	for (auto sq : Squares)
	{
		delete sq.second;
	}
	Squares.clear();
	m_CameraOrientation->SetState(GameObject::DEAD);
}

void Board::SetPieceOnBadStartSquare(Piece* piece)
{
	Square* square = GetBadStartSquare();
	piece->m_FootOnSquare = square;
	square->m_Piece = piece;

	D3DXVECTOR3 pos = square->m_SquareObject->GetTransform()->GetWorldPosition() +
		OFFSET_PIECE_ON_SQUARE;
	piece->m_PieceObject->SetPositionIncludeChilds(pos);
}

void Board::SetPieceOnGoodStartSquare(Piece* piece)
{
	Square* square = GetGoodStartSquare();
	piece->m_FootOnSquare = square;
	square->m_Piece = piece;

	D3DXVECTOR3 pos = square->m_SquareObject->GetTransform()->GetWorldPosition() +
		OFFSET_PIECE_ON_SQUARE;
	piece->m_PieceObject->SetPositionIncludeChilds(pos);
}

Square* Board::GetBadStartSquare()
{
	Square* square=nullptr;
	for (auto sq : BadStartSquare)
	{
		if (sq->m_Piece == nullptr)
		{
			square = sq;
			break;
		}
	}
	return square;
}

Square* Board::GetGoodStartSquare()
{
	Square* square = nullptr;
	for (auto sq : GoodStartSquare)
	{
		if (sq->m_Piece == nullptr)
		{
			square = sq;
			break;
		}
	}
	return square;
}

Square* Board::GetPrisonRoomSquare()
{
	Square* square = nullptr;
	for (auto sq : PrisonRoomSquare)
	{
		if (sq->m_Piece == nullptr)
		{
			square = sq;
			break;
		}
	}
	return square;
}
