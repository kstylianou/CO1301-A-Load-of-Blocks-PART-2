//Kyriakos Stylianou
//G 20795768
//kstylianou1@uclan.ac.uk

// StylianouKyriakos.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream>
#include <cmath>
#include <math.h>
#include <time.h>
#include "StylianouKyriakos_marble.h"
#include "StylianouKyriakos_block.h"
#include "StylianouKyriakos_barrier.h"
#include "StylianouKyriakos_cam.h"
using namespace tle;
using namespace std;

//Angry Block to Block Collision
bool updateAngryBlock(IModel* angryBlock, IModel* block, int angryBoxR) {
	//Angry Block r + block r
	float xMax = angryBlock->GetX() + angryBoxR;
	float xMin = angryBlock->GetX() - angryBoxR;
	float yMax = angryBlock->GetY() + angryBoxR;
	float yMin = angryBlock->GetY() - angryBoxR;
	float zMax = angryBlock->GetZ() + angryBoxR;
	float zMin = angryBlock->GetZ() - angryBoxR;

	float x = block->GetX();
	float y = block->GetY();
	float z = block->GetZ();

	//Collision detection
	if (x > xMin && x < xMax && y >= yMin && y< yMax && z > zMin && z < zMax) {
		return true;
	}
}

//Marble to marble collision detection
void marbleToMarbleCollision(IModel* marble, Cmarble& m, int marbles) {
	float x;
	//Random Skin
	m.SetRandomSkin(rand() % 3 + 1);
	float collisionDist;
	float Cmx = marble->GetX() - m.GetMarble(marbles)->GetX();
	float Cmy = marble->GetY() - m.GetMarble(marbles)->GetY();
	float Cmz = marble->GetZ() - m.GetMarble(marbles)->GetZ();

	//The lenght of 2 marbles
	collisionDist = sqrt(Cmx * Cmx + Cmy * Cmy + Cmz * Cmz);

	if (collisionDist < m.GetMarbleR() + m.GetMarbleR()) {
		//Make the bounce
		x = m.GetRad(marbles);
		m.SetRad(marbles, x = -m.GetRad(marbles));

		if (m.GetRandomSkin() == m.GetSkin1()) {
			marble->SetSkin("glass_green.jpg");
		}
		if (m.GetRandomSkin() == m.GetSkin2()) {
			marble->SetSkin("glass_blue.jpg");
		}
		if (m.GetRandomSkin() == m.GetSkin3()) {
			marble->SetSkin("glass_red.jpg");
		}
	}


}

//Collision detection marble with walls
bool blockCollition(IModel* wall, IModel* marble, int marbleBlockR) {
	//Block r + marble r
	float xMax = wall->GetX() + marbleBlockR;
	float xMin = wall->GetX() - marbleBlockR;
	float yMax = wall->GetY() + marbleBlockR;
	float yMin = wall->GetY() - marbleBlockR;
	float zMax = wall->GetZ() + marbleBlockR;
	float zMin = wall->GetZ() - marbleBlockR;

	float x = marble->GetX();
	float y = marble->GetY();
	float z = marble->GetZ();

	//Collision detection
	if (x > xMin && x < xMax && y >= yMin && y< yMax && z > zMin && z < zMax) {
		return true;
	}

}

//Return the number of removed blocks
int blockCollisionNum(Cblock& b) {
	b.SetBlockDeleted(0);
	//Return the number of removed blocks
	for (int i = 0; i < b.GetMaxBlocks(); i++) {
		if (b.GetCblock(i) == b.GetDeleteBlockNum()) {
			b.SetBlocksDeleted(b.GetBlocksDeleted() + 1);
		}
	}
	return b.GetBlocksDeleted();
}

//Update Marble Rotation
void updateMurbleRotation(I3DEngine* myEngine, IModel* dummy, Cmarble& m, float& dt) {
	//marble speed using variable time
	float vMarble = m.GetStepMarbleAngle() * dt;
	//Rotate dummy , arrow left
	if (myEngine->KeyHeld(Key_Left))
	{
		if (m.GetMarbleAngle() >= -m.GetMarbleAngleLimit()) {

			m.SetMarbleAngle(m.GetMarbleAngle() - vMarble);
			dummy->RotateY(-vMarble);
		}
	}

	//Rotate dummy , arrow right
	if (myEngine->KeyHeld(Key_Right)) {
		if (m.GetMarbleAngle() <= m.GetMarbleAngleLimit()) {
			m.SetMarbleAngle(m.GetMarbleAngle() + vMarble);
			dummy->RotateY(vMarble);

		}
	}
}

//update Block movements and rows
void updateBlock(Cblock& b, float dt, IMesh* mBlock, bool& gameOver) {
	//Degrees for block 
	b.SetDegrees(-54);

	//Moving all the blocks Z
	for (int i = 0; i < b.GetKblock(); i++) {
		if (b.GetCblock(i) != b.GetDeleteBlockNum()) {
			b.GetBlock(i)->MoveZ(-b.GetStepBlock() * dt);

			//When the block is less than Z(5) the game is over
			if (b.GetBlock(i)->GetZ() < b.GetMinBlockZ()) {
				gameOver = true;
			}
		}

	}
	//Get the position of the blocks if is less than 120 new block created
	for (int i = b.GetKblock() - b.GetBlockRow(); i < b.GetKblock(); i++) {
		if (b.GetCblock(i) != b.GetDeleteBlockNum()) {
			if (b.GetBlock(i)->GetZ() < b.GetBlockMinZ() && b.GetKblock() != b.GetMaxBlocks()) {
				b.SetKblock(b.GetKblock() + b.GetBlockRow());

				//1 random hard block for evey row not 5th
				if (i < b.GetHardBlockMin() || i > b.GetHardBlockMax()) {
					b.SetRandHardBlock(rand() % b.GetBlockRow() + b.GetKblock() - 11);
				}
				//1 random sticky block for every 5th row
				else {
					b.SetRandStickyBlock(rand() % b.GetBlockRow() + b.GetKblock() - 11);
				}
				b.SetNewBlock(true);
				break;

			}
		}
	}
	//Create new raw of blocks
	if (b.GetNewBlocks() == true) {
		for (int i = b.GetKblock() - b.GetBlockRow(); i < b.GetKblock(); i++) {

			b.SetNewBlock(mBlock, i, b.GetDegrees(), 0, 132);
			b.SetDegrees(b.GetDegrees() + 12);


			if (i == b.GetRandHardBlock()) {

				b.GetBlock(b.GetRandHardBlock())->SetSkin("tiles_half.jpg");
				b.SetCblock(b.GetRandHardBlock(), 3);
			}
			if (i == b.GetStickyBlock()) {

				b.GetBlock(b.GetStickyBlock())->SetSkin("tiles_pink.jpg");
				b.SetCblock(b.GetStickyBlock(), 7);
			}
		}
		b.SetNewBlock(false);
	}

	//When the game is over all the block are red
	if (gameOver == true) {
		for (int i = 0; i < b.GetKblock(); i++) {
			if (b.GetCblock(i) != b.GetDeleteBlockNum()) {
				b.GetBlock(i)->SetSkin("tiles_red.jpg");
			}
		}
	}

}

//update block FSM
void collisionDetection(IMesh* mBlock, Cmarble& m, Cblock& b, float dt, int marbles) {

	switch (b.GetBlockState(marbles))
	{
	case STATE_COLLISION:

		for (int i = 0; i < b.GetKblock(); i++) {
			//Check if there is collision with marble
			if (b.GetCblock(i) != b.GetDeleteBlockNum() && b.GetCblock(i) != b.GetStickyBlockNum()) {
				if (blockCollition(b.GetBlock(i), m.GetMarble(marbles), b.GetMarbleBlockR()) == true) {
					b.SetCblock(i, b.GetCblock(i) + 1);
					m.SetBlockChangeSkin(marbles, i);

					if (b.GetCblock(i) == b.GetStickyBlockNum()) {
						//Time starts 
						b.SetStart();
						b.SetTimeToGetStick();
					}
					else {
						//Move marble Backwards
						m.SetMarbleHit(marbles, true);
					}

					b.SetBlockState(marbles, STATE_CHANGE_SKIN);
					break;
				}
			}
		}
		break;

	case STATE_CHANGE_SKIN:
		//When it is hit only 1 change to red
		if (b.GetCblock(m.GetBlockChangeSkin(marbles)) == b.GetChangeSkinNum()) {
			b.GetBlock(m.GetBlockChangeSkin(marbles))->SetSkin("tiles_red.jpg");
		}

		//When it is hit only 2 remove from game
		if (b.GetCblock(m.GetBlockChangeSkin(marbles)) == b.GetDeleteBlockNum()) {
			b.SetBlockState(marbles, STATE_BLOCK_DELETE);
		}
		//When the hard block it get hit 3 times remove from game
		else if (b.GetCblock(m.GetBlockChangeSkin(marbles)) == b.GetHardBlockNum()) {
			b.SetBlockState(marbles, STATE_BLOCK_DELETE);
			b.SetCblock(m.GetBlockChangeSkin(marbles), b.GetDeleteBlockNum());
		}
		//When there is collision with sticky marble stops for 1 second and remove the block
		else if (b.GetCblock(m.GetBlockChangeSkin(marbles)) == b.GetStickyBlockNum()) {
			m.SetState(marbles, STATE_STICK);
			b.SetEnd();

			if (b.GetElapsed() >= b.GetSeconds()) {
				b.SetBlockState(marbles, STATE_BLOCK_DELETE);
				m.SetState(marbles, STATE_FIRING);
				b.SetCblock(m.GetBlockChangeSkin(marbles), b.GetDeleteBlockNum());

				//marble’s speed be halved once released
				m.SetMarbleHit(marbles, true);
			}
		}
		else if (b.GetCblock(m.GetBlockChangeSkin(marbles)) >= b.GetAngryBlockMinHit() && b.GetCblock(m.GetBlockChangeSkin(marbles)) <= b.GetAngryBlockMaxHit()) {
			b.GetBlock(m.GetBlockChangeSkin(marbles))->Scale(b.GetAngryBlockScale());
			b.SetBlockState(marbles, STATE_COLLISION);
			if (b.GetCblock(m.GetBlockChangeSkin(marbles)) == b.GetAngryBlockMaxHit()) {

				for (int i = 0; i < b.GetKblock(); i++) {
					if (b.GetCblock(i) != b.GetDeleteBlockNum() && i != m.GetBlockChangeSkin(marbles)) {
						//If there is collision destroys their neighbours, before sinking into the ground
						if (updateAngryBlock(b.GetBlock(m.GetBlockChangeSkin(marbles)), b.GetBlock(i), b.GetAngryBoxSize()) == true) {

							//Remove Model
							b.SetModelToRemove(mBlock, i);
							//Set the delete block
							b.SetCblock(i, b.GetDeleteBlockNum());
						}
					}
				}

				b.SetCblock(m.GetBlockChangeSkin(marbles), b.GetDeleteBlockNum());
				b.SetBlockState(marbles, STATE_BLOCK_DELETE);
			}

		}
		else {
			b.SetBlockState(marbles, STATE_COLLISION);
		}
		break;

	case STATE_BLOCK_DELETE:
		//sink slowly into the ground before it is deleted.
		b.GetBlock(m.GetBlockChangeSkin(marbles))->MoveY(-b.GetVblock() * dt);
		//when the block is sink into the ground remove it from the game
		if (b.GetBlock(m.GetBlockChangeSkin(marbles))->GetY() < b.GetMinSink()) {
			b.SetModelToRemove(mBlock, m.GetBlockChangeSkin(marbles));
			m.SetBlockChangeSkin(marbles, 0);
			b.SetBlockState(marbles, STATE_COLLISION);
		}
		break;
	}
}

//Update Marble FSM
void updateMarble(I3DEngine* myEngine, IMesh* mBlock, Cmarble& m, Cblock& b, IModel* dummy, Cbarrier barrier, double PI, float dt, int& marbles, bool& gameOver) {

	//Marble velocity using variable time
	float vMarble = m.GetDs(marbles) * dt;
	int marbleX = 0;
	float rad;

	switch (m.GetState(marbles))
	{
	case STATE_READY:

		if (myEngine->KeyHit(Key_Space)) {
			m.SetState(marbles, STATE_FIRING);
			rad = m.GetMarbleAngle() * PI / 180;
			m.SetRad(marbles, rad);
			m.SetMarbleX(marbles, 0);

			//Moving marble X -10 units 
			for (int i = 0; i < m.GetKMarble(); i++) {
				if (m.GetMarbleReady(i) != true) {
					//marblePosition[i]->SetPosition(marbleXpossition[marbles] * -10, 0, 0);
					m.SetMarblePosition(i, m.GetMarbleX(marbles) * -10, 0, 0);
					//marbleXpossition[marbles]++;
					m.SetMarbleX(marbles, m.GetMarbleX(marbles) + 1);
				}
			}
		}
		break;

	case STATE_FIRING:

		//if hit is false moving the murble forword else is moving backwords
		if (m.GetMarbleHit(marbles) == false) {
			m.GetMarble(marbles)->Move(m.GetRad(marbles) * vMarble, 0, m.GetZmove(marbles) * vMarble);
			m.GetMarble(marbles)->RotateX(m.GetZmove(marbles) * vMarble);
		}
		else {
			m.GetMarble(marbles)->Move(m.GetRad(marbles) * vMarble, 0, -m.GetZmove(marbles) * vMarble);
			m.GetMarble(marbles)->RotateX(-m.GetZmove(marbles) * vMarble);
		}

		//Marble is moving is true 
		m.SetMarbleMoving(marbles, true);


		// collision detection and resolution between the marbles
		for (int i = 0; i < m.GetKMarble(); i++) {
			if (i != marbles && m.GetMarbleMoving(i) == true) {
				marbleToMarbleCollision(m.GetMarble(i), m, marbles);
			}
		}
		// if marble exceeds 200 set to (0,0,0)
		if (m.GetMarble(marbles)->GetZ() >= m.GetMaxDistanceBullet()) {
			m.SetState(marbles, STATE_CONTACT);

		}
		//Check if there is collision with barrier 
		for (int i = 0; i < barrier.GetKbarrier(); i++) {
			if (blockCollition(barrier.GetBarrierRight(i), m.GetMarble(marbles), b.GetMarbleBlockR()) == true || blockCollition(barrier.GetBarrierLeft(i), m.GetMarble(marbles), b.GetMarbleBlockR()) == true) {
				// bounce off surfaces at an appropriate angle
				m.SetRad(marbles, -m.GetRad(marbles));
			}
		}

		//when the z-coordinate of the marble becomes 0 go to state Contact to set position
		if (m.GetMarble(marbles)->GetLocalZ() < m.GetMarbleMinDistance()) {
			m.SetState(marbles, STATE_CONTACT);
		}
		break;

	case STATE_CONTACT:

		// Set marble position X else hit becomes true and state = firing
		if (m.GetMarble(marbles)->GetZ() < m.GetMarbleMinZ() || m.GetMarble(marbles)->GetZ() >= m.GetMaxDistanceBullet()) {

			m.SetMarbleX(marbles, 0);

			for (int i = 0; i < m.GetKMarble(); i++) {
				if (m.GetMarbleMoving(i) != true) {
					m.SetMarbleX(marbles, m.GetMarbleX(marbles) + 1);
				}

				//if all the block are removed game is over
				if (blockCollisionNum(b) == b.GetMaxBlocks()) {
					m.SetState(i, STATE_OVER);
				}
			}

			m.GetMarble(marbles)->SetPosition(m.GetMarbleX(marbles) * -10, 0, 0);
			m.SetMarbleMoving(marbles, false);
			m.SetState(marbles, STATE_READY);
			m.SetMarbleReady(marbles, false);
			m.SetMarbleHit(marbles, false);
		}
		else {
			m.SetState(marbles, STATE_FIRING);
			m.SetMarbleHit(marbles, true);
		}
		break;

	case STATE_OVER:
		//At state over all the marbles skin is green
		for (int i = 0; i < m.GetKMarble(); i++) {
			m.GetMarble(i)->SetSkin("glass_green.jpg");
		}
		gameOver = true;
		break;

	case STATE_STICK:
		//if there is collision whith sticky box
		break;
	}
}

//Update game controls
void updateGame(I3DEngine* myEngine) {
	//Escape the game
	if (myEngine->KeyHit(Key_Escape)) {
		myEngine->Stop();
	}
}
//Update camera position
void updateCamera(Ccam& cam, I3DEngine* myEngine, float& dt) {

	if (myEngine->KeyHit(Key_1)) {
		//Up, down key are now moving the camera
		cam.SetChangeCamPosition(true);
	}
	if (myEngine->KeyHeld(Key_Up) && cam.GetChangeCamPosition() == true) {

		//Moving cameras Yaxis
		if (cam.GetCam()->GetY() <= cam.GetCamMaxY()) {
			cam.GetCam()->MoveY(cam.GetYds() * dt);
		}
		//Moving cameras Zaxis
		if (cam.GetCam()->GetZ() <= cam.GetCamMaxZ() && cam.GetCam()->GetY() > cam.GetCamMinZY()) {
			cam.GetCam()->MoveZ(cam.GetZds() * dt);
		}

		//Rotate camera
		if (cam.GetCamAngle() <= cam.GetCamAngleLimit()) {
			cam.SetCamAngle(cam.GetCamAngle() + cam.GetXds() * dt);
			cam.GetCam()->RotateX(cam.GetXds() * dt);
		}


	}

	if (myEngine->KeyHeld(Key_Down) && cam.GetChangeCamPosition() == true) {
		//Moving cameras Yaxis
		if (cam.GetCam()->GetY() >= cam.GetCamMinY()) {
			cam.GetCam()->MoveY(-cam.GetYds() * dt);
		}
		//Moving cameras Zaxis
		if (cam.GetCam()->GetZ() >= cam.GetCamMinZ()) {
			cam.GetCam()->MoveZ(-cam.GetZds() * dt);
		}
		//Rotate camera
		if (cam.GetCamAngle() >= cam.GetCamAngleLimitMin()) {
			cam.SetCamAngle(cam.GetCamAngle() - cam.GetXds() * dt);
			cam.GetCam()->RotateX(-cam.GetXds() * dt);
		}
	}

	//Reset camera to defult position
	if (myEngine->KeyHit(Key_C)) {
		cam.GetCam()->SetPosition(0, 30, -60);
		cam.GetCam()->RotateX(-cam.GetCamAngle());
		cam.SetCamAngle(0.0);
	}

	//Top view of the camera
	if (myEngine->KeyHit(Key_2)) {
		cam.GetCam()->SetPosition(0, 160, 60);
		cam.SetCamRPositions(cam.GetCamAngleLimit() - cam.GetCamAngle());
		cam.GetCam()->RotateX(cam.GetCamRposition());
		cam.SetCamAngle(80.0);
		cam.SetChangeCamPosition(false);
	}
}

//Update the scene for every frame
void updateScene(I3DEngine* myEngine, Ccam& cam, IMesh* mBlock, Cmarble& m, Cblock& b, IModel* dummy, Cbarrier& barrier, float& dt, double PI, int& marbles, bool& gameOver) {

	//variable timing
	dt = myEngine->Timer();

	// Check if more than 1 marble is at the same position when is not moving 
	int marbleAtSamePosition = 0;

	//loop for the update marble
	for (int i = 0; i < m.GetKMarble(); i++) {
		//When the marble is ready X(0) it call the function UpdateMarble and CollisionDetection
		if (m.GetMarbleReady(i) == true && gameOver == false) {
			updateMarble(myEngine, mBlock, m, b, dummy, barrier, PI, dt, i, gameOver);
			collisionDetection(mBlock, m, b, dt, i);
		}

		//If marble is at position x(0) it become ready for fire
		if (m.GetMarble(i)->GetX() == m.GetMarbleMinDistance() && m.GetMarbleReady(i) == false) {
			m.SetMarbleReady(i, true);
		}

		//If both marble are in the same possition X 
		if (m.GetMarble(i)->GetX() == m.GetMarbleSecondPosition() && m.GetMarbleMoving(i) == false) {
			++marbleAtSamePosition;
		}
		// set it to X(0) to get ready
		if (marbleAtSamePosition > m.GetMarbleAtSamePosition()) {
			if (m.GetMarble(i)->GetX() == m.GetMarbleSecondPosition()) {
				m.GetMarble(i)->SetPosition(0, 0, 0);
				marbleAtSamePosition = m.GetMarbleMinDistance();
			}
		}

	}



	//If the game is not over call the functions
	if (gameOver == false) {
		updateBlock(b, dt, mBlock, gameOver);
		updateMurbleRotation(myEngine, dummy, m, dt);
		updateCamera(cam, myEngine, dt);
	}

	//Update game controls
	updateGame(myEngine);
}


void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("./media");

	/**** Set up your scene here ****/
	IMesh* mfloor = myEngine->LoadMesh("Floor.x");
	IMesh* mSkybox = myEngine->LoadMesh("Skybox_Hell.x");
	IMesh* mBlock = myEngine->LoadMesh("Block.x");
	IMesh* mMarble = myEngine->LoadMesh("Marble.x");
	IMesh* mArrow = myEngine->LoadMesh("Arrow.x");
	IMesh* mBarrier = myEngine->LoadMesh("Barrier.x");
	IMesh* mDummy = myEngine->LoadMesh("Dummy.x");
	IModel* floor = mfloor->CreateModel(0, -5, 0);
	IModel* skybox = mSkybox->CreateModel(0, -1000, 0);
	IModel* arrow = mArrow->CreateModel(0, 0, -10);
	IModel* dummy = mDummy->CreateModel(0, 0, 0);

	// variable timing
	myEngine->Timer();
	float dt = myEngine->Timer();
	int marbles = 0;

	//When blocks are in position Z(0) or when all blocks are removed the game is over 
	bool gameOver = false;
	const double PI = 3.14159;
	Cmarble m = Cmarble(mMarble, PI);
	Cblock b = Cblock(mBlock);
	Cbarrier barrier = Cbarrier(mBarrier);
	Ccam cam = Ccam(myEngine);
	//  Attach the arrow to the marble
	arrow->AttachToParent(dummy);



	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();


		/**** Update your scene each frame here ****/

		updateScene(myEngine, cam, mBlock, m, b, dummy, barrier, dt, PI, marbles, gameOver);

	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
