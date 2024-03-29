#include "Cave.h"

MCave::MCave():MLevel() {
	FlagsMap = NULL;
	RandomPercent = 0;
	SmoothCycles = 0;
	WallsToSmooth = 0;
	WallTreshold = 0;
	RoomTreshold = 0;
}

MCave::MCave(int TilesCountX, int TilesCountY, int inRandomPercent, int inSmoothCycles, int inWallsToSmooth, int inWallTreshold, int inRoomTreshold):MLevel(TilesCountX, TilesCountY) {
	FlagsMap = NULL;
	RandomPercent = 0;
	SmoothCycles = 0;
	WallsToSmooth = 0;
	WallTreshold = 0;
	RoomTreshold = 0;
	if(inRandomPercent > 0) RandomPercent = inRandomPercent;
	if(inSmoothCycles > 0) SmoothCycles = inSmoothCycles;
	if(inWallsToSmooth > 0) WallsToSmooth = inWallsToSmooth;
	if(inWallTreshold > 0) WallTreshold = inWallTreshold;
	if(inRoomTreshold > 0) RoomTreshold = inRoomTreshold;
}

std::list<NVector2> MCave::GetRegion(int i, int j, char Value) {
	std::list<NVector2> Region;
	std::queue<NVector2> Queue;
	NVector2 Cell;
	
	Queue.push(NVector2(i, j));
	FlagsMap[i][j] = 1;
	
	while(Queue.size()) {
		Cell = Queue.front();
		Region.push_back(Cell);
		for (int x = Cell.x - 1; x <= Cell.x + 1; x++) {
			for (int y = Cell.y - 1; y <= Cell.y + 1; y++) {
				if(x < TilesCount[0] && y < TilesCount[1] && x > 0 && y > 0) {
					if(Map[x][y] == Value && !FlagsMap[x][y] && (x == Cell.x || y == Cell.y) ) {
						Queue.push(NVector2(x, y));
						FlagsMap[x][y] = 1;
					}
				}
			}
        }
		Queue.pop();
	}
	
	return Region;
}

bool MCave::SmoothMap() {
	//smooth map
	int WallsCount;
	for(int k=0; k<SmoothCycles; k++) {
		for(int i=1; i<TilesCount[0] - 1; i++) {
			for(int j=1; j<TilesCount[1] - 1; j++) {
				//get surround walls count
				WallsCount = Map[i-1][j-1] + Map[i][j-1] + Map[i+1][j-1] +
					Map[i-1][j] + Map[i+1][j] + 
					Map[i-1][j+1] + Map[i][j+1] + Map[i+1][j+1];
				if(WallsCount > WallsToSmooth) Map[i][j] = 1;
				else if(WallsCount < WallsToSmooth) Map[i][j] = 0;
			}
		}
	}
	
	return true;
}

bool MCave::CreateRegions() {
	//work with regions
	if(!FlagsMap) {
		FlagsMap = new char* [TilesCount[0]];
		for(int i=0; i<TilesCount[0]; i++) {
			FlagsMap[i] = new char [TilesCount[1]];
			memset(FlagsMap[i], 0, TilesCount[1]);
		}
	}
	else {
		for(int i=0; i<TilesCount[0]; i++) {
			memset(FlagsMap[i], 0, TilesCount[1]);
		}
	}
	
	//get regions
	for(int i=1; i<TilesCount[0] - 1; i++) {
		for(int j=1; j<TilesCount[1] - 1; j++) {
			if(FlagsMap[i][j] == 0 && Map[i][j] == 1) {
				RoomsRegionsList.push_back(GetRegion(i, j, 1));
			}
			if(FlagsMap[i][j] == 0 && Map[i][j] == 0) {
				WallsRegionsList.push_back(GetRegion(i, j, 0));
			}
		}
	}
	
	//remove small walls and rooms regions
	std::list<std::list<NVector2> >::iterator it;
	it = RoomsRegionsList.begin();
	while (it != RoomsRegionsList.end()) {
	    int Size = it->size();
	    if(Size < RoomTreshold) {
	    	for(std::list<NVector2>::iterator ita = it->begin(); ita != it->end(); ita++) {
				Map[ita->x][ita->y] = 0;
			}
	        RoomsRegionsList.erase(it ++);
	    }
	    else {
	        it ++;
	    }
	}
	it = WallsRegionsList.begin();
	while (it != WallsRegionsList.end()) {
	    int Size = it->size();
	    if(Size < WallTreshold) {
	    	for(std::list<NVector2>::iterator ita = it->begin(); ita != it->end(); ita++) {
				Map[ita->x][ita->y] = 0;
			}
	        WallsRegionsList.erase(it ++);
	    }
	    else {
	        it ++;
	    }
	}
	
	return true;
}

bool MCave::CreateHalls() {
	if(RoomsRegionsList.size() <= 1) {
		return true;
	}
	
	if(RoomsRegionsList.size() >= 2) {
		std::vector<glm::vec2> RandomPoints;
		std::list<NVector2>::iterator ita;
		std::vector<NVector2> LinePoints;
		
		//get random point from each of region
		for(std::list<std::list<NVector2> >::iterator it = RoomsRegionsList.begin(); it != RoomsRegionsList.end(); it++) {
			ita = it->begin();
			std::advance(ita, rand() % it->size());
			RandomPoints.push_back(glm::vec2(ita->x, ita->y));
		}
		if(RandomPoints.size() == 2) {
			LinePoints = CreateLine(RandomPoints[0].x, RandomPoints[0].y, RandomPoints[1].x, RandomPoints[1].y);
			for(int j=0; j<LinePoints.size(); j++) {
				Map[LinePoints[j].x][LinePoints[j].y] = 1;
				if(LinePoints[j].x < TilesCount[0] - 1  && LinePoints[j].y < TilesCount[1] - 1) {
					Map[LinePoints[j].x + 1][LinePoints[j].y] = 1;
					Map[LinePoints[j].x][LinePoints[j].y + 1] = 1;
					Map[LinePoints[j].x + 1][LinePoints[j].y + 1] = 1;
				}
			}
			RandomPoints.clear();
			LinePoints.clear();
			return true;
		}
		
		//triangulate by delaunay and get mst
		MDelaunay Triangulation;
		std::vector<MTriangle> Triangles = Triangulation.Triangulate(RandomPoints);
		std::vector<MEdge> Edges = Triangulation.GetEdges();
		std::vector<MEdge> MST = Triangulation.CreateMSTEdges();
		
		//connect points
		for(int i=0; i<MST.size(); i++) {
			LinePoints = CreateLine(MST[i].p1.x, MST[i].p1.y, MST[i].p2.x, MST[i].p2.y);
			if(!LinePoints.size()) {
				std::cout<<"Empty Line"<<std::endl;
				break;
			}
			for(int j=0; j<LinePoints.size(); j++) {
				Map[LinePoints[j].x][LinePoints[j].y] = 1;
				if(LinePoints[j].x < TilesCount[0] - 1  && LinePoints[j].y < TilesCount[1] - 1) {
					Map[LinePoints[j].x + 1][LinePoints[j].y] = 1;
					Map[LinePoints[j].x][LinePoints[j].y + 1] = 1;
					Map[LinePoints[j].x + 1][LinePoints[j].y + 1] = 1;
				}
			}
		}
		
		//clear
		LinePoints.clear();
		RandomPoints.clear();
		Triangulation.Clear();
		Triangles.clear();
		Edges.clear();
		MST.clear();
	}
	
	return true;
}

bool MCave::CreateEdges(int EdgeSize) {
	if(EdgeSize < 0) return false;
	for(int i=1; i<TilesCount[0]; i++) {
		for(int j=1; j<TilesCount[1]; j++) {
			if(i < EdgeSize || j < EdgeSize || i >= TilesCount[0] - EdgeSize || j >= TilesCount[1] - EdgeSize) Map[i][j] = 0;
		}
	}
	
	return true;
}

bool MCave::SetStartEndPoints() {
	if(!RoomsRegionsList.size()) return false;
	
	std::list<std::list<NVector2> >::iterator it;
	std::list<NVector2>::iterator itn;
	int RndPointNumber;
	int RndRoomsNumbers[2] = {0, 0};
	NVector2 EntryPoints[2] = NVector2(0, 0);
	
	//try to get two random values based on regions count
	//need to change alghoritm while regions count less than 3
	//maybe select first and last elements or just one element
	int Result = RamdomFromCount2(RoomsRegionsList.size(), RndRoomsNumbers[0], RndRoomsNumbers[1]);
	if(!Result) {
		std::cout<<"Regions count less than 3"<<std::endl;
		return false;
	}
	
	it = RoomsRegionsList.begin();
	std::advance(it, RndRoomsNumbers[0]);
	RndPointNumber = rand() % it->size();
	itn = it->begin();
	std::advance(itn, RndPointNumber);
	EntryPoints[0] = *itn;
	
	it = RoomsRegionsList.begin();
	std::advance(it, RndRoomsNumbers[1]);
	RndPointNumber = rand() % it->size();
	itn = it->begin();
	std::advance(itn, RndPointNumber);
	EntryPoints[1] = *itn;
	
	nStartPoint[0] = EntryPoints[0].x;
	nStartPoint[1] = EntryPoints[0].y;
	nEndPoint[2] = EntryPoints[1].x;
	nEndPoint[2] = EntryPoints[1].y;
	
	return true;
}

bool MCave::Generate() {
	if(!Map) {
		if(!AllocateMap()) {
			std::cout<<"Error while allocate map"<<std::endl;
			return false;
		}
	}
	else MLevel::Clear();
	
	//radomly fill map
	for(int i=1; i<TilesCount[0] - 1; i++) {
		for(int j=1; j<TilesCount[1] - 1; j++) {
			Map[i][j] = RandomChancePercent(RandomPercent);
		}
	}
	
	SmoothMap();
	if(!CreateRegions()) {
		std::cout<<"Error while create regions"<<std::endl;
		return false;
	}
	if(!CreateHalls()) {
		std::cout<<"Error while create halls"<<std::endl;
		return false;
	}
	SmoothMap();
	if(!CreateEdges(1)) return false;
	if(!ConvertToTiles()) return false;
	if(!SetStartEndPoints()) return false;
	
	RoomsRegionsList.clear();
	WallsRegionsList.clear();
	
	return true;
}

void MCave::Clear() {
	MLevel::Clear();
}

void MCave::Close() {
	//remove flags map
	for(int i=0; i<TilesCount[0]; i++) {
		if(FlagsMap[i]) delete [] FlagsMap[i];
	}
	if(FlagsMap) delete [] FlagsMap;
	
	MLevel::Close();
}

int MCave::GetType() {
	return LGT_CAVE;
}


