#include <iostream>
#include <SDL.h>
#include <ctime>
//TODO добавь лист удаленных элементов и переред отрисовкой удаляьть их 
#define NOTHING 0
#define GRASS 1
#define HERBIVORE 2
#define PREDATOR 3
#define STARTGRASSENERGY 50
#define HERBLENGHTOFVISION 100
#define PREDLENGHTOFVISION 200
SDL_Window* win = NULL;
SDL_Renderer* ren = NULL;
const int colCount = 64;
const int rowCount = 50;
int squareSize = 16;
int win_width = colCount * squareSize, win_height = rowCount * squareSize;
int FPS = 30;
float findmax(float a, float b) {
	return a > b ? a : b;
}
float findmin(float a, float b) {
	return a < b ? a : b;
}
void DeInit(int error) {
	if (ren != NULL) SDL_DestroyRenderer(ren);
	if (win != NULL) SDL_DestroyWindow(win);
	SDL_Quit();
	exit(error);
}
void Init() {
	
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("Couldn't init SDL! %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}


	win = SDL_CreateWindow("Just another window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		win_width, win_height, SDL_WINDOW_SHOWN );
	if (win == NULL) {
		printf("Couldn't get window! %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (ren == NULL) {
		printf("couldn't get renderer! %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}
}
class Cell {
protected:
	SDL_Point position;
	uint8_t type;
public:


	Cell(int positionx, int positiony, uint8_t type) {
		position.x = positionx;
		position.y = positiony;
		this->type = type;
	}
	SDL_Point getPosition() {
		return position;
	}
	~Cell() {
	}

};
class Alive {
protected:
	bool newborn;
	uint8_t energy;
	uint8_t maxenergy;
	uint8_t minenergy;
public:
	Alive(uint8_t energy, uint8_t maxenergy, uint8_t minenergy) {
		this->energy = energy;
		this->maxenergy = maxenergy;
		this->minenergy = minenergy;
		this->newborn = true;
	}
	uint8_t getEnergy() {
		return energy;
	}
	uint8_t getMaxEnergy() {
		return maxenergy;
	}
	uint8_t getMinEnergy() {
		return minenergy;
	}
	bool getNewBorn() {
		return newborn;
	}
	void makeOld() {
		newborn = false;
	}
	void makeNew() {
		newborn = true;
	}
	~Alive() {
	}
};
class Field {
private:
	int width;
	int height;
	uint8_t* cellContents;
	bool sunShine;
public:
	Field(int width, int height) {
		this->width = width;
		this->height = height;
		sunShine = true;
		cellContents = new uint8_t[width * height];
		memset(cellContents, 0, width * height * sizeof(uint8_t));
	}
	~Field() {
		if (cellContents != NULL) {
			delete[] cellContents;
		}
		
	}
	bool locating(int x, int y, uint8_t type) {
		bool result = true;
		if(type == NOTHING){
			cellContents[x + y * colCount] = NOTHING;
		}
		else {
			if (cellContents[x + y * colCount] != 0) {
				result = false;
			}
			else {
				cellContents[x + y * colCount] = type;
			}
		}
		
		
		return result;
	}
	bool getSunShine() {
		return sunShine;
	}
	bool changeSunShine() {
		sunShine = !sunShine;
		return sunShine;
	}
	void drawField()
	{
		
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		int xpos1, xpos2, ypos1, ypos2;
		xpos1 = ypos1 = 0;
		ypos2 = win_height;
		for (int i = 0; i < win_width / squareSize; i++) {
			xpos1 += 16;
			SDL_RenderDrawLine(ren, xpos1, ypos1, xpos1, ypos2);
		}
		xpos1 = ypos1 = 0;
		xpos2 = win_width;
		for (int i = 0; i < win_height / squareSize; i++) {
			ypos1 += 16;
			SDL_RenderDrawLine(ren, xpos1, ypos1, xpos2, ypos1);
		}
	}
	int getWidth() { return width; }
	int getHeight() { return height; }

};
class Grass: public  Cell,public Alive {
private: 
	Field* parentField;
	uint8_t startEnergy;
	uint8_t hydrophilious;
	uint8_t childhyd;
	uint8_t childrud;
	uint8_t rudeness;
	bool changeGen() {
		bool result;
		uint8_t randomize = rand() % 256;
		result = randomize % 10 == 0;
		if (result) {
			randomize = rand() % 256;
			if (randomize % 2 == 0) {
				
				if (randomize % 4 == 0) {
					if (childrud > 245) {
						childrud = 25;
					}
					else {
					
					childrud = rudeness + 8;
					}
				}
				else {
					if (childrud < 20) {
						childrud = 240;
					}
					else {
					childrud = rudeness - 8;
					}
				}
			}
			else {
				randomize = rand() % 256;
				if (randomize % 2 == 0) {
					childhyd = hydrophilious + 8;
				}
				else {
					childhyd = hydrophilious - 8;
				}
			}
		}
		
		return result;
	}

public:

	Grass(int positionx,int positiony,uint8_t maxEnergy,uint8_t startEnergy,uint8_t minEnergy,uint8_t hydrophilious,uint8_t rudeness,Field* parent):
		Cell(positionx,positiony,GRASS),
		Alive(startEnergy,maxEnergy,minEnergy) {
		this->hydrophilious = hydrophilious;
		this->rudeness = rudeness;
		this->startEnergy = startEnergy;
		this->childhyd = hydrophilious;
		this->childrud = rudeness;
		this->parentField = parent;
	}
	void printGrass() {
		printf("grass haracteristics: \n");
		printf("Position: (%d,%d)\n", position.x, position.y);
		printf("hydraphilious: %d\n rudeness: %d\nstartEnergy: %d,minEnergy: %d,maxenergy: %d\n curenergy: %d\n", hydrophilious, rudeness, startEnergy, minenergy, maxenergy, energy);
	}
	Grass* reproduction(int childposx, int childposy) {
		changeGen();
		energy = maxenergy - startEnergy;
		Grass* child = new Grass(childposx, childposy, childrud, childrud / 2, childrud / 3, childhyd, childrud, parentField);
		return child;
	}
	int nextStep() {
		int result = 0;
		if (energy >= maxenergy){
			result = 1;
		}
		else if (energy < minenergy) {
			result = 2;
		}
		else {
			result = 3;
		}
		
		return result;
	}
	uint8_t getRudeness() {
		return rudeness;
	}
	int changeEnergy(bool sunshine,bool change) {
		//в центре поля находиться водоем от гена hydrophilious зависит то сколько бонусов будет получать трава от того что находиться ближе к водоему
		//но если трава находиться далеко от центра она получает больше энергии
		//если она засухоустойчевая т.е. чем меньше hydrophilious тем больше энергии она получит
		float radius = sqrt(win_width / 4 * win_width  + win_height / 4 * win_height);
		float xrel = win_width / 2 - position.x;
		float yrel = win_height / 2 - position.y;
		float len = sqrt(xrel * xrel + yrel * yrel);
		float hydbonus = (hydrophilious * (1 - (len / radius))) / 2;
		float drybonus = ((255-hydrophilious)* (1-((radius-len)/radius))) /2;
		float changing = (hydbonus + drybonus) / 50;
		if (change) {
			if (sunshine) {
				if (energy + changing < maxenergy) {
					energy += changing;
				}
				else {
					energy = maxenergy;
				}
			}
			else {
				float multiplier = 399.F / 400.F;
				float curenergy = energy;
				energy = (uint8_t)(curenergy * multiplier);
			}
		}
		
		return changing;
	}
	void addEnergy(int getingenergy) {
		if (getingenergy > 0) {
			if (getingenergy > maxenergy - energy) {
				energy = maxenergy;
			}
			else {
				energy += getingenergy;
			}
		}
		else {
			if (getingenergy * (-1) > energy) {
				energy = 0;
			}
			else {
				energy += getingenergy;
			}
		}
	}
	bool death() {
		return parentField->locating(position.x, position.y, NOTHING);
	}
	bool getFreePosition(int* positionx, int* positiony) {
		int result = rand() % 8;
		int attempts = 0;
		int nearpositionx;
		int nearpositiony;
		if (*positionx == 0) {
			nearpositionx = colCount-1;
		}
		else {
			nearpositionx = *positionx - 1;
		}
		if (*positiony == 0) {
			nearpositiony = rowCount-1;
		}
		else {
			nearpositiony = *positiony - 1;
		}
		bool find = false;
		while (true) {
			switch (result) {
			case 0:

				
				if (parentField->locating(nearpositionx,nearpositiony, GRASS)) {
					find = true;
					*positionx = nearpositionx;
					*positiony = nearpositiony;
				}
				break;
			case 1:
				if (parentField->locating(*positionx, nearpositiony, GRASS)) {
					find = true;
					*positiony = nearpositiony;
				}
				break;
			case 2:
				if (parentField->locating((*positionx + 1)%colCount, nearpositiony, GRASS)) {
					find = true;
					*positionx = (*positionx + 1) % colCount;
					*positiony = nearpositiony;
				}
				break;
			case 3:
				if (parentField->locating(nearpositionx, *positiony, GRASS)) {
					find = true;
					*positionx = nearpositionx;
				}
				break;
			case 4:
				if (parentField->locating((*positionx + 1)%colCount, *positiony, GRASS)) {
					find = true;
					*positionx = (*positionx + 1) % colCount;
				}
				break;
			case 5:
				if (parentField->locating(nearpositionx, (*positiony + 1)%rowCount, GRASS)) {
					find = true;
					*positionx = nearpositionx;
					*positiony = (*positiony + 1) % rowCount;
				}
				break;
			case 6:
				if (parentField->locating(*positionx, (*positiony + 1) % rowCount, GRASS)) {
					*positiony = (*positiony + 1) % rowCount;
					find = true;
				}

				break;
			case 7:
				if (parentField->locating((*positionx + 1) % colCount, (*positiony + 1) % rowCount, GRASS)) {
					find = true;
					*positionx = (*positionx + 1) % colCount;
					*positiony = (*positiony + 1) % rowCount;
				}
				break;

			default:
				attempts = 7;
				break;
			}
			result = (result + 1) % 8;
			if (++attempts == 8 || find) break;
		}
		return find;
	}
	Field* getParentField() {
		return parentField;
	}
	~Grass() {
	}
};
class Herbivore : public Cell {
private:
	uint8_t childrud;
	uint8_t childspeed;
	uint8_t rudeness;
	uint8_t speed;
	bool canmove;
	bool findgrass;
	int targetposx;
	int targetposy;
	float energy;
	int startenergy;
	int maxenergy;
	int minenergy;
	bool newborn;
	bool changeGen() {
		bool result;
		uint8_t randomize = rand() % 256;
		result = randomize % 10 == 0;
		//с вероятностью 10% ребенок меняет один из двух генов влияющих на его характеристики
		if (result) {
			randomize = rand() % 256;
			if (randomize % 2 == 0) {

				if (randomize % 4 == 0) {
					childrud = rudeness + 8;
				}
				else {
					childrud = rudeness - 8;
				}
			}
			else {
				randomize = rand() % 256;
				if (randomize % 2 == 0) {
					if (speed > 253) {
						childspeed = 50;
					}
					else {
						childspeed = speed + 2;
					}
				}
				else {
					if (childspeed < 52) {
						childspeed = 250;
					}
					else {
						childspeed = speed - 2;
					}
					
				}
			}
		}

		return result;
	}

public:
	Herbivore(int positionx,int positiony,uint8_t rudeness,uint8_t speed):
		Cell(positionx,positiony,HERBIVORE)
		{
		this->energy = rudeness;
		this->startenergy = rudeness;
		this->maxenergy = rudeness*2;
		this->minenergy = rudeness*2/3;
		this->targetposx = 0;
		this->targetposy = 0;
		this->canmove = true;
		this->rudeness = rudeness;
		this->speed = speed;
		this->childrud = rudeness;
		this->childspeed = speed;
		this->findgrass = false;
		this->newborn = true;
	}
	bool getNewBorn() {
		return newborn;

	}
	void makeOld() {
		newborn = false;
	}
	void makeNew() {
		newborn = true;
	}
	float getEnergy() {
		return energy;
	}
	int getMaxEnergy() {
		return maxenergy;
	}
	int getMinEnergy() {
		return minenergy;
	}
	bool getFindGrass() {
		return findgrass;
	}
	void setFindGrass(bool findgrass) {
		this->findgrass = findgrass;
	}
	int getTargetPosx() {
		return targetposx;
	}
	int getTargetPosy() {
		return targetposy;
	}
	void setTargetPosx(int targetposx) {
		this->targetposx = targetposx;
	}
	void setTargetPosy(int targetposy) {
		this->targetposy = targetposy;
	}
	bool changeMove() {
		canmove  = !canmove;
		return canmove;
	}
	bool getMove() {
		return canmove;
	}
	bool move(int* posxh, int* posyh,int posxp,int posyp ,bool direction) {
		// функция предназначена для передвижения клеток по экрану
		bool result = false;
		// вычисляем разницу меджу координатами
		float positionx = *posxh;
		float positiony = *posyh;
		float xrel = posxp - positionx ;
		float yrel = posyp - positiony;
		// находим длину вектора
		float len = sqrt(xrel * xrel + yrel * yrel); 
		// находим растояние которое объект моет пройти за 1 кадр
		float speedperframe = speed;
		speedperframe /= FPS;
		// если идем к объекту
		if (direction) {
			if (canmove) {
				if (len > speedperframe) {
					// нормализуем и изменяем разницу по х и у
					xrel /= len;
					yrel /= len;
					positionx += xrel * speedperframe;
					positiony += yrel * speedperframe;

				}
				else {
					//если растояние между коорди натами меньше чем растояние которое объект 
					//преодаливает за кадр то приравниваем координаты
					positionx = posxp;
					positiony = posyp;
					//меняем canmove так чтобы объект не мог двигаться пока ест
					canmove = false;
					*posxh = positionx;
					*posyh = positiony;
					result = true;
					return result;
				}
			}
			else {
				canmove = true;
			}
		}
		//если идем от объекта
		else {
			if (canmove) {
				if (len == 0) len = 1;
				xrel /= len;
				yrel /= len;
				positionx -= xrel * speedperframe;
				positiony -= yrel * speedperframe;
			}
			else {
				canmove = true;
			}
			

		}
		// checking for minimum and maximum value
		if (!direction) {
			//чтобы не убегал за пределы поля от хищника
			positionx = findmin(win_width, positionx);
			positionx = findmax(0, positionx);
			positiony = findmin(win_height, positiony);
			positiony = findmax(0, positiony);
		}
		
		*posxh = positionx;
		*posyh = positiony;
		return result;
	}
	uint8_t getRudeness() {
		return rudeness;
	}
	bool checkPos(int posxpred, int posypred,bool direction) {
		//вызывает move если объект в зоне видимости
		float x = position.x;
		float y = position.y;
		float posxpredf = posxpred;
		float posypredf = posypred;
		float xrel = x - posxpredf;
		float yrel = y - posypredf;
		bool isnear = sqrt(xrel * xrel + yrel * yrel) < HERBLENGHTOFVISION;
		if (isnear) {
			move(&position.x, &position.y, posxpred, posypred,direction);
			chgEnSp();
		}
		
		return isnear;
	}
	void chgEnSp() {
		//уменьшает энергию во время энергии
		float changing = FPS;
		changing /= 1000;
		changing *= speed/2;
		energy -= changing;

	}
	void setPosition(int x, int y) {
		position.x = x;
		position.y = y;
	}
	bool eat(Grass* grass) {
		// уменьшаем здоровье grass и увеличиваем здоровье herbivore 
		// в зависимости от отношения их генов отвечающих за здоровье
		int changing;
		float herbrudeness = this->getRudeness();
		float grassrudeness = grass->getRudeness();
		float rudmultiplyer = (herbrudeness / grassrudeness) / 30;
		changing = grass->getEnergy() * rudmultiplyer;
		if (changing > maxenergy - energy) {
			energy = maxenergy;
		}
		else {
			energy += changing*2;
		}
		grass->addEnergy(changing * (-2));
		return grass->getEnergy() <= grass->getMinEnergy();
	}
	Herbivore* reproduction() {
		//меняет ген ребенка
		changeGen();
		int childposx = this->getPosition().x;
		int childposy = this->getPosition().y;
		//смещаем координаты ребенка к центру
		move(&childposx, &childposy, win_width / 2, win_height / 2, true);
		Herbivore* child = new Herbivore(childposx, childposy, childrud, childspeed);
		this->energy = startenergy;
		return child;
	}
	void addEnergy(int getingenergy) {
		if (getingenergy > 0) {
			if (getingenergy > maxenergy - energy) {
				energy = maxenergy;
			}
			else {
				energy += getingenergy;
			}
		}
		else {
			if (getingenergy * (-1) > energy) {
				energy = 0;
			}
			else {
				energy += getingenergy;
			}
		}
	}
	~Herbivore() {
	}
	void printHerb(){
		printf("herbivore haracteristics:\n");
		if (newborn) {
			printf("newborn\n");
		}
		else {
			printf("old\n");
		}
		printf("positionx = %d, positiony = %d\n", position.x, position.y);
		printf("energy = %f\n speed = %d, rudenes = %d\n,targetposx = %d, targetposy = %d\n maxen = %d, minen = %d\n ",energy,speed,rudeness,targetposx,targetposy,maxenergy,minenergy);
		if (canmove) {
			printf("canmove, ");
		}
		else {
			printf("cant move, ");
		}
		if (findgrass) {
			printf("have target\n");
		}
		else {
			printf("have null target\n");
		}
	}
	
};
struct HerbNode {

	Herbivore* val;
	HerbNode* next;
	HerbNode(Herbivore* _val) : val(_val), next(nullptr) {}
};
class HerbList {
private:
	HerbNode* first;
	HerbNode* last;
	int size;
public:
	HerbList() {
		first = nullptr;
		last = nullptr;
		size = 0;
	}
	bool isEmpty() {
		return first == nullptr;
	}
	void pushBack(Herbivore* _val) {
		HerbNode* p = new HerbNode(_val);
		size++;
		if (isEmpty()) {
			first = p;
			last = p;
			return;
		}
		last->next = p;
		last = p;
		
	}
	HerbNode* find(Herbivore* _val) {
		if (isEmpty()) return nullptr;
		HerbNode* p = first;
		while (p && p->val != _val) p = p->next;
		return (p && p->val == _val) ? p : nullptr;
	}
	void removeFirst() {
		if (isEmpty()) return;
		size--;
		if (first == last) {
			first = nullptr;
			last = nullptr;
			return;
		}
		HerbNode* p = first;
		first = p->next;

	}
	void removeLast() {
		if (isEmpty()) return;
		if (first == last) {
			removeFirst();
			return;
		}
		HerbNode* p = first;
		while (p->next != last) p = p->next;
		p->next = nullptr;
		
		last = p;
		size--;
	}
	bool remove(Herbivore* _val) {
		bool result = true;
		if (isEmpty()) return false;
		if (first->val == _val) {
			removeFirst();
			return result;
		}
		else if (last->val == _val) {
			removeLast();
			return result;
		}
		HerbNode* slow = first;
		HerbNode* fast = first->next;

		while (fast && fast->val != _val) {
			slow = slow->next;
			fast = fast->next;
		}
		if (fast) {
			slow->next = fast->next;			
			size--;
		}
		else {
			result = false;
		}
		return result;

	}
	HerbNode*  get(int index) {
		if (isEmpty()) return nullptr;
		HerbNode* p = first;
		for (int i = 0; i < index; i++) {
			p = p->next;
			if (!p) return nullptr;
		}
		return p;
	}

	int getSize() {
		return size;
	}
	void checkingForMin() {

		if (isEmpty()) {
			return;
		}
		while (first && first->val->getEnergy() <= first->val->getMinEnergy()) {
			delete first->val;
			removeFirst();
		}
		while (last && last->val->getEnergy() <= last->val->getMinEnergy()) {
			delete last->val;
			removeLast();
		}
		if (isEmpty()) return;
		HerbNode* slow = first;
		HerbNode* fast = first->next;
		if (isEmpty()) return;
		while (fast) {
			while (fast && fast->val->getEnergy() > fast->val->getMinEnergy()) {
				slow = slow->next;
				fast = fast->next;
			}
			if (fast) {
				slow->next = fast->next;
				delete fast->val;
				fast = fast->next;
				size--;
			}
		}
		return;
		
	}
	
};
class Predator : public Cell {
private:
	uint8_t childrud;
	uint8_t childspeed;
	uint8_t rudeness;
	uint8_t speed;
	bool canmove;
	float energy;
	int startenergy;
	int maxenergy;
	int minenergy;
	bool newborn;
	Herbivore* target;
	bool changeGen() {
		bool result;
		uint8_t randomize = rand() % 256;
		result = randomize % 10 == 0;
		if (result) {
			randomize = rand() % 256;
			if (randomize % 2 == 0) {

				if (randomize % 4 == 0) {
					childrud = rudeness + 8;
				}
				else {
					childrud = rudeness - 8;
				}
			}
			else {
				randomize = rand() % 256;
				if (randomize % 2 == 0) {
					if (speed > 253) {
						childspeed = 50;
					}
					else {
						childspeed = speed + 2;
					}
				}
				else {
					if (childspeed < 52) {
						childspeed = 250;
					}
					else {
						childspeed = speed - 2;
					}

				}
			}
		}

		return result;
	}
public:
	Predator(int positionx, int positiony,uint8_t rudeness, uint8_t speed) :Cell(positionx, positiony, PREDATOR)
	{
		this->childrud = rudeness;
		this->childspeed = speed;
		this->rudeness = rudeness;
		this->speed = speed;
		this->canmove = true;
		this->energy = rudeness*3;
		this->startenergy = rudeness*3;
		this->maxenergy = rudeness *6;
		this->minenergy = rudeness * 2;
		this->newborn = true;
		this->target = nullptr;
	}
	void printPred() {
		printf("predator haracteristics:\n");
		if (newborn) {
			printf("newborn\n");
		}
		else {
			printf("old\n");
		}
		printf("positionx = %d, positiony = %d\n", position.x, position.y);
		printf("energy = %f\n speed = %d, rudenes = %d\n maxen = %d, minen = %d\n ", energy, speed, rudeness, maxenergy, minenergy);
		if (canmove) {
			printf("canmove, ");
		}
		else {
			printf("cant move, ");
		}
		if (target) {
			printf("target position (%d,%d)\n", target->getPosition().x, target->getPosition().y);
		}
		else {
			printf("still seraching\n");
		}
	}
	bool getNewBorn() {
		return newborn;

	}
	void makeOld() {
		newborn = false;
	}
	void makeNew() {
		newborn = true;
	}
	float getEnergy() {
		return energy;
	}
	int getMaxEnergy() {
		return maxenergy;
	}
	int getMinEnergy() {
		return minenergy;
	}
	bool changeMove() {
		canmove = !canmove;
		return canmove;
	}
	bool getMove() {
		return canmove;
	}
	uint8_t getRudeness() {
		return rudeness;
	}
	Herbivore* getTarget() {
		return target;
	}
	void setTarget(Herbivore* target){
		this->target = target;
	}
	void targetNull() {
		target = nullptr;
	}
	bool eat() {
		//уменьшаем здоровье target(herbivore) и увеличиваем здоровье predator
		// в зависимости от генов отвечающих за здоровье
		int changing;
		float predrudeness = this->getRudeness();
		float targetrudeness = target->getRudeness();
		float rudmultiplyer = (predrudeness / targetrudeness) / 30;
		changing = target->getEnergy() * rudmultiplyer;
		if (changing > maxenergy - energy) {
			energy = maxenergy;
		}
		else {
			energy += changing * 2;
		}
		target->addEnergy(changing * (-2));
		return target->getEnergy() <= target->getMinEnergy();
		return true;
	}
	bool move(int* posxh, int* posyh, int posxp, int posyp) {
		bool result = false;
		float positionx = *posxh;
		float positiony = *posyh;
		float xrel = posxp - positionx;
		float yrel = posyp - positiony;
		float len = sqrt(xrel * xrel + yrel * yrel);
		float speedperframe = speed;
		speedperframe /= FPS;
		if (canmove) {
			if (len > speedperframe) {

				xrel /= len;
				yrel /= len;
				positionx += xrel * speedperframe;
				positiony += yrel * speedperframe;

			}
			else {

				positionx = posxp;
				positiony = posyp;
				if (target) {
					target->changeMove();
				}
				canmove = false;
				*posxh = positionx;
				*posyh = positiony;
				result = true;
				return result;
			}
		}
		else {
			canmove = true;
		}
		

		*posxh = positionx;
		*posyh = positiony;
		return result;
	}
	Predator* reproduction() {
		changeGen();
		int childposx = this->getPosition().x;
		int childposy = this->getPosition().y;
		move(&childposx, &childposy, win_width, win_height);
		this->energy = startenergy;
		Predator* child = new Predator(childposx,childposy,childrud,childspeed);
		return child;
	}
	bool checkPos(Herbivore* herb) {
		//вызывает move если цель в зоне видимости
		float x = position.x;
		float y = position.y;
		float posxherb = herb->getPosition().x;
		float posyherb = herb->getPosition().y;
		float xrel = x - posxherb;
		float yrel = y - posyherb;
		bool isnear = sqrt(xrel * xrel + yrel * yrel) < PREDLENGHTOFVISION;
		if (isnear) {
			move(&position.x, &position.y, posxherb, posyherb);
			chgEnSp();
		}

		return isnear;
	}
	void chgEnSp() {
		float changing = FPS;
		changing /= 1000;
		changing *= (speed / 4);
		energy -= changing;

	}
	void setPosition(int x, int y) {
		position.x = x;
		position.y = y;
	}
	
	~Predator() {
	}
};
struct PredNode {

	Predator* val;
	PredNode* next;
	PredNode(Predator* _val) : val(_val), next(nullptr) {}
};
class PredList {
private:
	PredNode* first;
	PredNode* last;
	int size;
public:
	PredList() {
		first = nullptr;
		last = nullptr;
		size = 0;
	}
	bool isEmpty() {
		return first == nullptr;
	}
	void pushBack(Predator* _val) {
		
		PredNode* p = new PredNode(_val);
		size++;
		if (isEmpty()) {
			first = p;
			last = p;
			return;
		}
		last->next = p;
		last = p;

	}
	PredNode* find(Predator* _val) {
		if (isEmpty()) return nullptr;
		PredNode* p = first;
		while (p && p->val != _val) p = p->next;
		return (p && p->val == _val) ? p : nullptr;
	}
	void removeFirst() {
		if (isEmpty()) return;
		size--;
		if (first == last) {
			first = nullptr;
			last = nullptr;
			return;
		}
		PredNode* p = first;
		first = p->next;
	}
	void removeLast() {
		if (isEmpty()) return;
		if (first == last) {
			removeFirst();
			return;
		}
		PredNode* p = first;
		while (p->next != last) p = p->next;
		p->next = nullptr;
		last = p;
		size--;
	}
	bool remove(Predator* _val) {
		bool result = true;
		if (isEmpty()) return false;
		if (first->val == _val) {
			removeFirst();
			return result;
		}
		else if (last->val == _val) {
			removeLast();
			return result;
		}
		PredNode* slow = first;
		PredNode* fast = first->next;

		while (fast && fast->val != _val) {
			slow = slow->next;
			fast = fast->next;
		}
		if (fast) {
			slow->next = fast->next;
			size--;
		}
		else {
			result = false;
		}
		return result;

	}
	PredNode* get(const int index) {
		if (isEmpty()) return nullptr;
		PredNode* p = first;
		for (int i = 0; i < index; i++) {
			p = p->next;
			if (!p) return nullptr;
		}
		return p;
	}
	int getSize() {
		return size;
	}
	void checkingForMin() {

		if (isEmpty()) {
			return;
		}
		while (first && first->val->getEnergy() <= first->val->getMinEnergy()) {
			delete first->val;
			removeFirst();
		}
		while (last && last->val->getEnergy() <= last->val->getMinEnergy()) {
			delete last->val;
			removeLast();
		}
		if (isEmpty()) return;
		PredNode* slow = first;
		PredNode* fast = first->next;

		while (fast) {
			while (fast && fast->val->getEnergy() > fast->val->getMinEnergy()) {
				slow = slow->next;
				fast = fast->next;
			}
			if (fast) {
				slow->next = fast->next;
				delete fast->val;
				fast = fast->next;
				size--;
			}
		}
		return;

	}


};
// функции для распределения энергии между соседями после смерти
void energyDistribution(int positionx, int positiony, Grass* grasarr[], int addingenergy);
void addEnergyToNear(Grass* near, int addingenergy);
int main(int argc, char* argv[])
{
#pragma region INITIALIZING
	srand(time(0));
	Init();
	SDL_Event ev;
	Field* field = new Field(colCount,rowCount);
	Grass* grasarr[rowCount*colCount];
	for (int i = 0; i < rowCount * colCount; i++) {
		grasarr[i] = NULL;
	}
	bool isRunning = true;
	grasarr[10+10*colCount] = new Grass(10, 10, 120, 120/2, 120/3, 125, 120, field);
	grasarr[11 + 11 * colCount] = new Grass(11, 11, 120, 120 / 2, 120 / 3, 125, 120, field);
	HerbList* herblist = new HerbList();
	Herbivore* herbivore = new Herbivore(14 * squareSize, 12 * squareSize, 150, 50);
	//Predator* predator = new Predator(16 * squareSize, 12 * squareSize, 150, 90);
	PredList* predlist = new PredList();
	//predlist->pushBack(predator);
	Grass* target;
	herblist->pushBack(herbivore);
	bool findgrass = false;
	uint8_t fromwhere = -1;
	bool stay;
	int herblistsize = herblist->getSize();
	int predlistsize = predlist->getSize();
	Herbivore* herb = nullptr;
	Predator* pred = nullptr;
	Herbivore* brother = nullptr;
	Grass* checkgrass = nullptr;
	int posx = 0;
	int posy = 0;
	int xrelative;
	int yrelative;
	int grasx;
	int grasy;
	int beginx;
	int beginy;
	Grass* curgras;
	bool issomeone;
	int lastbutton = -1;
#pragma endregion
	while (isRunning) {
#pragma region EVENTPROCESSING
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_KEYDOWN:
				switch (ev.key.keysym.scancode) {
				case SDL_SCANCODE_ESCAPE:
					isRunning = false;
					break;
				case SDL_SCANCODE_P:
					//spawn one predator (randomly)
					posx = rand() % win_width;
					posy = rand() % win_height;
					predlist->pushBack(new Predator(posx, posy, 150, 100));
					lastbutton = SDL_SCANCODE_P;
					break;
				case SDL_SCANCODE_H:
					//spawn one herbivore (randomly)
					posx = rand() % win_width;
					posy = rand() % win_height;
					herblist->pushBack(new Herbivore(posx, posy, 150, 50));
					lastbutton = SDL_SCANCODE_H;
					break;
				case SDL_SCANCODE_C:
					field->changeSunShine();
					break;
				case SDL_SCANCODE_G:
					//spawn grass (randomly)
					do {
					posx = rand() % colCount;
					posy = rand() % rowCount;
					
					} while (grasarr[posx + posy*colCount] != nullptr);
					grasarr[posx + posy * colCount] = new Grass(posx, posy, 120, 120 / 2, 120 / 3, 125, 120, field);
					field->locating(posx, posy, GRASS);
					lastbutton = SDL_SCANCODE_G;
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (ev.button.button == SDL_BUTTON_LEFT) {
					issomeone = false;
					for (int i = 0; i < predlistsize; i++) {
						pred = predlist->get(i)->val;
						xrelative = ev.button.x - pred->getPosition().x;
						yrelative = ev.button.y - pred->getPosition().y;
						if (xrelative <= squareSize && xrelative >= 0) {
							if (yrelative <= squareSize && yrelative >= 0) {
								pred->printPred();
								issomeone = true;
							}
						}
					}
					for (int i = 0; i < herblistsize; i++) {
						herb = herblist->get(i)->val;
						xrelative = ev.button.x - herb->getPosition().x;
						yrelative = ev.button.y - herb->getPosition().y;
						if (xrelative <= squareSize && xrelative >= 0) {
							if (yrelative <= squareSize && yrelative >= 0) {
								herb->printHerb();
								issomeone = true;
							}
						}
					}
					
					if (!issomeone) {
						switch (lastbutton) {
						case SDL_SCANCODE_H:
							//spawn one herbivore
							herblist->pushBack(new Herbivore(ev.button.x, ev.button.y, 150, 50));
							break;
						case SDL_SCANCODE_P:
							//spawn one predator
							predlist->pushBack(new Predator(ev.button.x, ev.button.y, 150, 100));
							break;
						case SDL_SCANCODE_G:
							//spawn grass (randomly)
							if (grasarr[ev.button.x / 16 + ev.button.y / 16 * colCount] == nullptr) {
								grasarr[ev.button.x / 16 + ev.button.y / 16 * colCount] = new Grass(posx, posy, 120, 120 / 2, 120 / 3, 125, 120, field);
								field->locating(posx, posy, GRASS);
							}
							break;
						default:
							break;
						}
					}

				}
				else if (ev.button.button == SDL_BUTTON_RIGHT) {
					target = grasarr[ev.button.x / 16 + ev.button.y / 16 * colCount];
					if (target) {
						target->printGrass();
					}
				}

			default:
				break;
			}
		}
#pragma endregion
#pragma region PROCESSING
		


#pragma region GRASSPROCESSING
		for (int i = 0; i < colCount; i++) {
			for (int j = 0; j < rowCount; j++) {
				curgras = grasarr[i + j * colCount];
				if (curgras != NULL) {
					//если появилась на предыдущем ходе то пропускаем этот ход и меняем флаг
					if (curgras->getNewBorn()) {
						curgras->makeOld();
					}
					else {

						SDL_Point position = { curgras->getPosition() };

						int stepresult = curgras->nextStep();
						switch (stepresult) {
							// если накопилось достаточно энергии то трава делиться
						case 1:
							
							if (curgras->getFreePosition(&position.x, &position.y)) {
								grasarr[position.x + position.y * colCount] =
									curgras->reproduction(position.x, position.y);
							}
							break;
						case 2:
							//если умирает то распределяет остатки энегрии между соседями
							energyDistribution(curgras->getPosition().x, curgras->getPosition().y, grasarr, curgras->getEnergy() / 8);
							curgras->death();
							delete curgras;
							grasarr[position.x + position.y * colCount] = NULL;
							break;
						case 3:
							//если сейчас день то трава увеличивает энергию
							curgras->changeEnergy(curgras->getParentField()->getSunShine(), true);
							break;
						default:
							break;
						}
					}
				}
			}
		}
#pragma endregion

#pragma region HERBIVOREPROCESSING
	herblistsize = herblist->getSize();
	predlistsize = predlist->getSize();
	for (int i = 0; i < herblistsize; i++) {
		herb = herblist->get(i)->val;
		
		if (herb->getNewBorn()) {
			herb->makeOld();
			break;
		}
		//checking for predator near
		stay = true;
		for (int j = 0; j < predlistsize; j++) {
			pred = predlist->get(j)->val;
			if (herb->checkPos(pred->getPosition().x, pred->getPosition().y, false)) {
				herb->setFindGrass(false);
				stay = false;
				break;
			}
			
		}
		//checking for grass near
		if (stay) {
			if (herb->getFindGrass()) {
				stay = false;
				target = grasarr[herb->getTargetPosx() + herb->getTargetPosy() * colCount];
				if (herb->getMove()) {			
					if (target != nullptr) {
						herb->checkPos(target->getPosition().x * squareSize,target->getPosition().y * squareSize, true);
					}
					else {
						herb->setFindGrass(false);
					}
				}
				else {
					if (target != nullptr) {
						if (herb->eat(target)) {
							energyDistribution(target->getPosition().x, target->getPosition().y, grasarr, target->getEnergy() / 8);
							target->death();
							delete target;
							grasarr[herb->getTargetPosx() + herb->getTargetPosy() * colCount] = NULL;
							herb->changeMove();
							herb->setFindGrass(false);
						}
						if (herb->getEnergy() >= herb->getMaxEnergy()) {
							herblist->pushBack(herb->reproduction());
							herb->makeNew();
						}

					}
					else {
						herb->setFindGrass(false);
					}
				}
				
			}
			else  {
				grasx = herb->getPosition().x / squareSize;
				grasy = herb->getPosition().y / squareSize;
				beginx = grasx;
				beginy = grasy;
				for (int i = 1; i < HERBLENGHTOFVISION / squareSize; i++) {
					grasx = grasx - i;
					grasy = grasy - i;
					for (int j = 0; j < i * 2; j++) {
						if (grasx >= 0 && grasx < colCount && grasy >= 0 && grasy < rowCount) {
							checkgrass = grasarr[grasx + grasy * colCount];
							if (checkgrass != NULL) {
								if (herb->checkPos(checkgrass->getPosition().x * squareSize, checkgrass->getPosition().y * squareSize, true)) {
									herb->setFindGrass(true);
									herb->setTargetPosx(grasx);
									herb->setTargetPosy(grasy);
									stay = false;
									break;
								}
							}
						}
						grasx++;

					}
					if (!stay) break;
					for (int j = 0; j < i * 2; j++) {
						if (grasx >= 0 && grasx < colCount && grasy >= 0 && grasy < rowCount) {
							checkgrass = grasarr[grasx + grasy * colCount];
							if (checkgrass != NULL) {
								if (herb->checkPos(checkgrass->getPosition().x * squareSize, checkgrass->getPosition().y * squareSize, true)) {
									herb->setFindGrass(true);
									herb->setTargetPosx(grasx);
									herb->setTargetPosy(grasy);
									stay = false;
									break;
								}
							}
						}
						grasy++;
					}
					if (!stay) break;
					for (int j = 0; j < i * 2; j++) {
						if (grasx >= 0 && grasx < colCount && grasy >= 0 && grasy < rowCount) {
							checkgrass = grasarr[grasx + grasy * colCount];
							if (checkgrass != NULL) {
								if (herb->checkPos(checkgrass->getPosition().x * squareSize, checkgrass->getPosition().y * squareSize, true)) {
									herb->setFindGrass(true);
									herb->setTargetPosx(grasx);
									herb->setTargetPosy(grasy);
									stay = false;
									break;
								}
							}
						}
						grasx--;
					}
					if (!stay) break;
					for (int j = 0; j < i * 2; j++) {
						if (grasx >= 0 && grasx < colCount && grasy >= 0 && grasy < rowCount) {
							checkgrass = grasarr[grasx + grasy * colCount];
							if (checkgrass != NULL) {
								if (herb->checkPos(checkgrass->getPosition().x * squareSize, checkgrass->getPosition().y * squareSize, true)) {
									herb->setFindGrass(true);
									herb->setTargetPosx(grasx);
									herb->setTargetPosy(grasy);
									stay = false;
									break;
								}
							}
						}
						grasy--;
					}
					if (!stay) break;
					grasx = beginx;
					grasy = beginy;
				}

			}
			
		}
		if (stay) {
		//checking for relatives near
			for (int j = 0; j < herblistsize; j++) {
				if (j != i) {
					brother = herblist->get(j)->val;
					if (herb->checkPos(brother->getPosition().x, brother->getPosition().y, true)) {
						stay = false;
						break;
					}
				}
			}
		}
		//going to center
		//can't die while going to center 
		if (stay) {
			posx = herb->getPosition().x;
			posy = herb->getPosition().y;
			if (herb->move(&posx, &posy, win_width / 2, win_height / 2, true)) {
				herb->changeMove();
			}
			herb->setPosition(posx, posy);

		}
	}
	herblist->checkingForMin();

#pragma endregion


#pragma region PREDPROCESSING
	herblistsize = herblist->getSize();
	predlistsize = predlist->getSize();
	for (int i = 0; i < predlistsize; i++) {
		pred = predlist->get(i)->val;
		if (pred->getNewBorn()) {
			pred->makeOld();
			break;
		}
		stay = true;
		if (!herblist->find(pred->getTarget())) {
			pred->targetNull();
		}
		if (pred->getTarget()) {
			stay = false;
			if (pred->getMove()) {
				pred->checkPos(pred->getTarget());

			}
			else {
				if (pred->eat()) {
					pred->changeMove();
					pred->targetNull();
				}
				if (pred->getEnergy() >= pred->getMaxEnergy()) {
					predlist->pushBack(pred->reproduction());
					pred->makeNew();
				}
				
			}
		}
		else {
			for (int j = 0; j < herblistsize; j++) {
				herb = herblist->get(j)->val;
				if (pred->checkPos(herb)) {
					pred->setTarget(herb);
					stay = false;
					break;
				}
				if (!stay) break;
			}
		}
		if (stay) {
			//going to center
			//cant die while going to center
			posx = pred->getPosition().x;
			posy = pred->getPosition().y;
			if (pred->move(&posx, &posy, win_width / 2, win_height / 2)) {
				pred->changeMove();
			}
			pred->setPosition(posx, posy);
		}

	}
	herblist->checkingForMin();
	predlist->checkingForMin();
#pragma endregion
#pragma endregion
#pragma region DRAWING
		//clear screen before new frame
		SDL_SetRenderDrawColor(ren, 175, 175, 175, 255);
		SDL_RenderClear(ren);
#pragma region GRASSDRAWING
		SDL_Rect rec;
		SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
		for (int i = 0; i < colCount; i++) {
			for (int j = 0; j < rowCount; j++) {
				if (grasarr[i + j * colCount] != NULL) {
					rec = { grasarr[i + j * colCount]->getPosition().x*squareSize,grasarr[i + j * colCount]->getPosition().y*squareSize,squareSize,squareSize };
					SDL_SetRenderDrawColor(ren, 0, 255 - grasarr[i + j * colCount]->getMaxEnergy(), 0, 255);
					SDL_RenderFillRect(ren, &rec);
				}
			}
		}
#pragma endregion		
			
		

		// grid drawing
		field->drawField();
		herblistsize = herblist->getSize();
		predlistsize = predlist->getSize();
#pragma region ALIVECELLDRAWING
		for (int i = 0; i < herblistsize; i++) {
			herb = herblist->get(i)->val;
			rec = {herb->getPosition().x,herb->getPosition().y,squareSize,squareSize};
			SDL_SetRenderDrawColor(ren,255-herb->getMaxEnergy(), 255 - herb->getMaxEnergy(),0, 255);
			SDL_RenderFillRect(ren, &rec);
		}

		for (int i = 0; i < predlistsize; i++) {

			pred = predlist->get(i)->val;
			rec = { pred->getPosition().x,pred->getPosition().y,squareSize,squareSize };
			SDL_SetRenderDrawColor(ren, pred->getRudeness(), 0, 0, 255);
			SDL_RenderFillRect(ren, &rec);
		}
#pragma endregion

#pragma endregion		Y
		SDL_RenderPresent(ren);
		SDL_Delay(1000/FPS);
	}
	DeInit(0);
	return 0;
}
void energyDistribution(int positionx,int positiony,Grass* grasarr[],int addingenergy) {
	int nearpositionx;
	int nearpositiony;
	Grass* curgras = grasarr[positionx + positiony * colCount];
	Field* field = curgras->getParentField();
	if (positionx == 0) {
		nearpositionx = colCount-1;
	}
	else {
		nearpositionx = positionx - 1;
	}
	if (positiony == 0) {
		nearpositiony = rowCount-1;
	}
	else {
		nearpositiony = positiony - 1;
	}

	Grass* neargrass = grasarr[nearpositionx + nearpositiony * colCount];
	addEnergyToNear(neargrass, addingenergy);

	neargrass = grasarr[positionx + nearpositiony * colCount];
	addEnergyToNear(neargrass, addingenergy);
	
	neargrass = grasarr[(positionx + 1)%colCount + nearpositiony * colCount];
	addEnergyToNear(neargrass, addingenergy);
	
	neargrass = grasarr[nearpositionx + positiony * colCount];
	addEnergyToNear(neargrass, addingenergy);
	
	neargrass = grasarr[(positionx + 1)%colCount + positiony * colCount];
	addEnergyToNear(neargrass, addingenergy);
	
	neargrass = grasarr[nearpositionx + ((positiony + 1)%rowCount) * colCount];
	addEnergyToNear(neargrass, addingenergy);
	neargrass = grasarr[positionx + ((positiony + 1)%rowCount) * colCount];
	addEnergyToNear(neargrass, addingenergy);
	
	neargrass = grasarr[(positionx + 1)%colCount + ((positiony + 1)%rowCount) * colCount];
	addEnergyToNear(neargrass, addingenergy);
}
void addEnergyToNear(Grass* near,int addingenergy) {
	if (near != NULL) {
		if (near->getEnergy() + addingenergy < near->getMaxEnergy()) {
			near->addEnergy(addingenergy);
		}
		else {
			near->addEnergy(near->getMaxEnergy() - near->getEnergy());
		}
	}
}
