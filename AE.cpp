#include <Windows.h>
#include <vector>
#include <string>
#include <iostream>
#include <random>
#include <utility>
#include <algorithm>
#include <fstream>
#include <math.h>
#include <time.h>
#include <ctime>
#include <chrono>

#include <opencv2/opencv.hpp>
#include <opencv2/text/ocr.hpp>

using namespace cv;
//using namespace dnn;
using namespace std;
using namespace cv::text;

enum botState { fishingState, grindingState, grindingLOMSlimeState, baruokiJumpRope, ratleJumpRope, silverHitBell_30, silverHitBell_999, seperateGrasta, engageFight, captureScreen};
botState runState = fishingState;

enum statusCode { status_NoError, status_Timeout, status_NotFight, status_FightFail, status_SlimeFound};
int m_lastGrasta = 0;
int m_SectionHeight = 133;
enum grastaType { grasta_Attack, grasta_Life, grasta_Support, grasta_Special};
int m_grastaType;
vector<string> m_grastaNames;

HWND window;

int M_WIDTH = (int) 1745.0;
int M_HEIGHT = (int) 981.0;
int M_WIDTH_1280 = (int) 1280.0;
int M_HEIGHT_720 = (int) 720.0;

int height, width;
int msdThreshold = 10000;
int loadTime = 3000;
bool m_IsDebug = false;
bool m_IsDebug_Path = false; 
bool m_IsDebug_Fighting = false;
bool m_IsDebug_Grinding = false;
bool m_IsDebug_Fishing = false;
bool m_IsDebug_Grasta = false;
bool m_IsDebug_LOM = false;

bool m_IsPrint = false;

double heightPct, widthPct;
WORD xCenter, yCenter;
Ptr<OCRTesseract> ocr;
HDC hdc;
HDC hDest;
HBITMAP hbDesktop;
Mat bitbltPic, fishIcon, exclamationIcon, exclRiftIcon, exclSepcterIcon, exclGrastaIcon, exclChamberIcon, exclChamberPlasmaIcon, exclKMSIcon, swampFishIcon, doorDefaultIcon, doorKunlunIcon, doorLOMIcon, stairDefaultIcon, stairDownIcon, stairLOMIcon;
Mat afBarEmptyPic, afBarFullPic, hitBellPic, jmpRopePic1, jmpRopePic2, jmpRopePic3, jmpRopePic4;

std::random_device dev;
std::mt19937 rng;
std::uniform_int_distribution<std::mt19937::result_type> shortSleepRand;
std::uniform_int_distribution<std::mt19937::result_type> lClickPixelRand;
std::uniform_int_distribution<std::mt19937::result_type> sleepRand;
std::uniform_int_distribution<std::mt19937::result_type> slideSleepRand;
std::uniform_int_distribution<std::mt19937::result_type> slideRand;
std::uniform_int_distribution<std::mt19937::result_type> slideDistanceRand;
std::uniform_int_distribution<std::mt19937::result_type> boolRand;
std::uniform_int_distribution<std::mt19937::result_type> slideLClick;
std::uniform_int_distribution<std::mt19937::result_type> longSleepRand;

string ocrNumbers = "1234567890,";
string ocrLetters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.'()";

vector<pair<int, int>> m_Locs_Acteul = { {900, 506}, {675, 377}, {1131, 392}, {657, 643}, {1130, 648}, };
vector<pair<int, int>> m_Locs_Baruoki = { {842, 508}, {632, 409}, {1047, 402}, {630, 634}, {1046, 623}, };
vector<pair<int, int>> m_Locs_DragonPalace = { {870, 483}, {654, 402}, {1055, 399}, {673, 580}, {1069, 596}, };
vector<pair<int, int>> m_Locs_Elzion = { {909, 517}, {691, 397}, {1154, 429}, {673, 643}, {1166, 659}, };
vector<pair<int, int>> m_Locs_DimensionRift = { {887, 481}, {604, 470}, {1112, 471} };
vector<pair<int, int>> m_Locs_KiraBeach = { {936, 533}, {697, 389}, {1180, 404}, {705, 686}, {1173, 687} };
vector<pair<int, int>> m_Locs_RucyanaSands = { {862, 494}, {650, 398}, {1080, 403}, {632, 603}, {1103, 622} };
vector<pair<int, int>> m_Locs_Vasu = { {879, 492}, {626, 395}, {1133, 399}, {612, 598}, {1166, 614} };
vector<pair<int, int>> m_Locs_Igoma = { {880, 510}, {600, 420}, {1135, 425}, {630, 610}, {1120, 620} };
vector<pair<int, int>> m_Locs_Moonlight = { {920, 435}, {635, 280}, {1215, 300}, {625, 580}, {1215, 575} };
vector<pair<int, int>> m_Locs_AncientBattlefield = { {890, 475}, {645, 385}, {1115, 385}, {660, 570}, {1135, 575} };
vector<pair<int, int>> m_Locs_ZolPlains = { {850, 515}, {560, 410}, {1115, 400}, {545, 640}, {885, 675} };
vector<pair<int, int>> m_Locs_Default = { {850, 490}, {700, 430}, {1000, 430}, {710, 570}, {880, 570} };
void(*fishIconClickFunction)() = NULL;


struct buttonInfo
{
	string buttonName;
	pair<int, int> xyPosition;
};

vector<buttonInfo> m_Button_MapButtons;
vector<pair<int, int>> m_Button_Characters = { { 135, 886 }, { 370, 886 }, { 605, 886 }, { 840, 886 }, {1075, 886}, {1310, 886} };
vector<pair<int, int>> m_Button_Skills = { { 195, 749 }, { 530, 749 }, { 865, 749 }, { 1200, 749 }, { 1400, 749 } };

int m_Skill_Normal = 0;
int m_Skill_Exchange = 4;
int m_Skill_Exchange_A = 4;
int m_Skill_Exchange_B = 5;
int m_Skill_AF = 6;
int m_Skill_for_AF = 3;
int m_CharacterFrontline = 4;

int m_Fight_HorrorCount = 2;
int m_Fight_GrindingStep = 1000;
int m_Fight_AFInterval = 100;
int m_Fight_AFFullThreshold = 1000;
int m_Fight_GrindingCount = 10;
int m_Fight_Timeout = 60;
bool m_Fight_GoToSpacetimeRift = true;
bool m_Fight_EndlessGrinding = false;

string m_Fight_LOMHeal;
string m_Fight_LOMSlimeA;
string m_Fight_LOMSlimeB;
string m_Fight_LOMSlimeRun;
string m_Fight_LOMReset;
int m_Fight_LOMTurn = 50;

int currentGrindingCounter;

vector<vector<int>> m_skillsHorrorSet;
vector<vector<int>> m_skillsMobSet;

bool isFightHorror = false;

char m_CurrentPath[MAX_PATH];

pair<int, int> m_Button_Yes = { 1068, 559 };
pair<int, int> m_Button_Leave = { 1560, 922 };
pair<int, int> m_Button_X = { 1699, 32 };
pair<int, int> m_Button_Menu = { 116, 886 };
pair<int, int> m_Button_Food = { 1384, 818 };
pair<int, int> m_Button_Attack = { 1595, 845 };
pair<int, int> m_Button_AF = { 1200, 60 };
pair<int, int> m_Button_Map = { 380, 900 };
pair<int, int> m_Button_Antiquity = { 116, 148 };
pair<int, int> m_Button_Present = { 279, 153 };
pair<int, int> m_Button_Future = { 444, 151 };
pair<int, int> m_Button_EndOfTimeLoc = { 610, 137 };
pair<int, int> m_Button_SpacetimeRift = { 873, 477 };
pair<int, int> m_Button_Tree = { 589, 78 };

vector<Mat>* currentMonsterVec = NULL;
vector<Mat> m_MonsterVec_Baruoki;
vector<Mat> m_MonsterVec_Acteul;
vector<Mat> m_MonsterVec_Vasu;
vector<Mat> m_MonsterVec_Serena;
vector<Mat> m_MonsterVec_Rucyana;
vector<Mat> m_MonsterVec_Elzion;
vector<Mat> m_MonsterVec_LastIsland;
vector<Mat> m_MonsterVec_DimensionRift;
vector<Mat> m_MonsterVec_LOMPSlime;

struct imageInfo
{
	string id;
	string name;
	int width;
	int height;
	int coordx;
	int coordy;
	Mat image;
};

vector<imageInfo> m_DynamicImage;

enum baitType { Fishing_Dango, Worm, Unexpected_Worm, Shopaholic_Clam, Spree_Snail, Dressed_Crab, Tear_Crab, Foamy_Worm, Bubbly_Worm, Snitch_Sardines, Blabber_Sardines, Crab_Cake, Premium_Crab_Cake };
enum doorType { doorDefault, doorKunlun, doorLOM};
enum stairType { stairDefault, stairDown, stairLOM };
enum exclType { exclDefault, exclRift, exclSepcter, exclGrasta, exclChamber, exclChamberPlasma, exclKMS};

struct fishingSpot
{
	fishingSpot(void(*fPtr)(), int orderNum, string locName) : fishFunction(fPtr), orderNumber(orderNum), locationName(locName) {}
	vector<baitType> baitsToUse;
	void(*fishFunction)();
	int orderNumber;
	string locationName;
};

struct pathInfo
{
	string type;
	string value1;
	string value2;
};

struct locationInfo
{
	string locationName;
	vector<pathInfo> pathStepsList;
};

vector<fishingSpot> m_fishingSpots;
vector<locationInfo> m_LocationList;
vector<pair<string, int>> m_grindingSpots;

vector<pair<bool, int>> m_baitList; //Index is the type, the boolean is whether or not you have greater than 0 currently held, int is how many to buy for this run
vector<baitType>* currentBaitsToUse;

pair<int, int> currentFishIconLoc;
string currentLocation;
bool hasHorror = false;

enum directionInfo
{
	LEFT, RIGHT, DOWN, UP, LR
};

directionInfo m_Fight_GrindingDirection = LEFT;

BOOL CALLBACK EnumWindowsProc(_In_ HWND hwnd, _In_ LPARAM lParam)
{
	DWORD dSize = MAX_PATH;
	TCHAR buffer[MAX_PATH] = { 0 };
	DWORD dwProcId = 0;

	GetWindowThreadProcessId(hwnd, &dwProcId);

	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);
	if (hProc)
	{
		QueryFullProcessImageName(hProc, 0, buffer, &dSize);
		CloseHandle(hProc);

		string windowTitle(buffer);

		string finalWindow = windowTitle.substr(windowTitle.rfind("\\") + 1, string::npos);

		if (finalWindow.compare((*((pair<string*, string*>*)(lParam))->first)) == 0)
		{
			GetWindowTextA(hwnd, buffer, MAX_PATH);
			windowTitle = buffer;

			if (windowTitle.compare((*((pair<string*, string*>*)(lParam))->second)) == 0)
			{
				window = hwnd;
				return false;
			}
		}
	}

	return true;
}

BOOL CALLBACK EnumChildWindowsProc(_In_ HWND hwnd, _In_ LPARAM lParam)
{
	TCHAR buffer[MAX_PATH] = { 0 };
	GetWindowTextA(hwnd, buffer, MAX_PATH);
	string windowTitle = buffer;

	if (windowTitle.compare((*((string*)(lParam)))) == 0)
	{
		window = hwnd;
		return false;
	}

	return true;
}

void sleepR(int time)
{
	Sleep(time + sleepRand(rng));
}

void longSleepR(int time)
{
	Sleep(time + longSleepRand(rng));
}

void rSlideSleep(int time)
{
	Sleep(time + slideSleepRand(rng));
}

void bitBltWholeScreen()
{
	BitBlt(hDest, 0, 0, width, height, hdc, 0, 0, SRCCOPY);
}

void copyPartialPic(Mat& partialPic, int cols, int rows, int x, int y)
{
	x = (int) round(x * widthPct);
	y = (int) round(y * heightPct);
	cols = (int) round(cols * widthPct);
	rows = (int) round(rows * heightPct);
	bitbltPic(cv::Rect(x, y, cols, rows)).copyTo(partialPic);
}

string runOCR(Mat& pic)
{
	vector<Rect>   boxes;
	vector<string> words;
	vector<float>  confidences;
	string output;
	Mat newPic;
	cvtColor(pic, newPic, COLOR_BGRA2BGR);
	ocr->run(newPic, output, &boxes, &words, &confidences, OCR_LEVEL_TEXTLINE);
	if (words.empty())
		return "";
	return words[0].substr(0, words[0].size() - 1);
}

string getText(Mat& pic)
{
	ocr->setWhiteList(ocrLetters);
	return runOCR(pic);
}

int getNumber(Mat& pic)
{
	ocr->setWhiteList(ocrNumbers);
	string strNum = runOCR(pic);

	//Remove any commas
	int pos = (int) strNum.find(',');
	while (pos != string::npos)
	{
		strNum.erase(pos, 1);
		pos = (int) strNum.find(',');
	}

	return stoi(strNum);
}

pair<int, int> findIcon(Mat& tmp)
{
	bitBltWholeScreen();
	int result_cols = bitbltPic.cols - tmp.cols + 1;
	int result_rows = bitbltPic.rows - tmp.rows + 1;
	Mat result;
	result.create(result_rows, result_cols, CV_32FC1);

	matchTemplate(bitbltPic, tmp, result, TM_SQDIFF_NORMED);

	Point minLoc, maxLoc;
	double minVal, maxVal;

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	return make_pair(minLoc.x, minLoc.y);
}

void leftClick(int x, int y, int sTime = 2000, bool changeLoc = true)
{
	if (changeLoc)
	{
		x = (int) round(x * widthPct);
		y = (int) round(y * heightPct);
	}

	int randX = (boolRand(dev) ? lClickPixelRand(rng) : ((-1) * lClickPixelRand(rng)));
	int randY = (boolRand(dev) ? lClickPixelRand(rng) : ((-1) * lClickPixelRand(rng)));
	SendMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(x + randX, y + randY));
	sleepR(10);
	SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(x + randX, y + randY));

	if (sTime <= 100)
		Sleep(sTime + shortSleepRand(rng));
	else
		Sleep(sTime + slideDistanceRand(rng));
}

void leftClick(pair<int, int>& coord, int sTime = 2000)
{
	leftClick(coord.first, coord.second, sTime);
}

void captureScreenNow(const char* nameSuffix = NULL)
{
	Mat imagePicCrop;
	char filename[MAX_PATH];
	struct tm timeinfo;
	char buf[80];
	time_t rawtime;

	rawtime = time(NULL);
	localtime_s(&timeinfo, &rawtime);
	strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);
	bitBltWholeScreen();
	copyPartialPic(imagePicCrop, M_WIDTH, M_HEIGHT, 0, 0);

	snprintf(filename, MAX_PATH, "%s\\%s_%s.jpg", m_CurrentPath, buf, nameSuffix);

	if (m_IsDebug) cout << filename << endl;
	imwrite(filename, imagePicCrop);
}

pair<int, int> findExclamationIcon(exclType whichExcl = exclDefault)
{
	pair<int, int> exclIcon;

	switch (whichExcl) {
	case exclRift:
		exclIcon = findIcon(exclRiftIcon);
		exclIcon.first += (int)round(50 * widthPct);
		exclIcon.second += (int)round(70 * heightPct);
		break;
	case exclSepcter:
		exclIcon = findIcon(exclSepcterIcon);
		exclIcon.first += (int)round(70 * widthPct);
		exclIcon.second += (int)round(95 * heightPct);
		break;
	case exclGrasta:
		exclIcon = findIcon(exclGrastaIcon);
		exclIcon.first += (int)round(50 * widthPct);
		exclIcon.second += (int)round(70 * heightPct);
		break;
	case exclChamber:
		exclIcon = findIcon(exclChamberIcon);
		exclIcon.first += (int)round(30 * widthPct);
		exclIcon.second += (int)round(50 * heightPct);
		break;
	case exclChamberPlasma:
		exclIcon = findIcon(exclChamberPlasmaIcon);
		exclIcon.first += (int)round(30 * widthPct);
		exclIcon.second += (int)round(50 * heightPct);
		break;
	case exclKMS:
		exclIcon = findIcon(exclKMSIcon);
		exclIcon.first += (int)round(17 * widthPct);
		exclIcon.second += (int)round(40 * heightPct);
		break;
	default:
		exclIcon = findIcon(exclamationIcon);
		exclIcon.first += (int)round(11 * widthPct);
		exclIcon.second += (int)round(25 * heightPct);
		break;
	}

	return exclIcon;
}

void findAndClickSwampFishIcon()
{
	pair<int, int> fishIconLoc = findIcon(swampFishIcon);
	fishIconLoc.first += (int) round(36 * widthPct);
	fishIconLoc.second += (int) round(19 * heightPct);
	leftClick(fishIconLoc.first, fishIconLoc.second, 2000, false);
}

void findAndClickFishIcon()
{
	pair<int, int> fishIconLoc = findIcon(fishIcon);
	fishIconLoc.first += (int) round(30 * widthPct);
	fishIconLoc.second += (int) round(15 * heightPct);
	leftClick(fishIconLoc.first, fishIconLoc.second, 2000, false);
}

pair<int, int> findDoorIcon(doorType whichDoor)
{
	pair<int, int> doorIconLoc;

	switch (whichDoor) {
	case doorKunlun:
		doorIconLoc = findIcon(doorKunlunIcon);
		doorIconLoc.first += (int)round(40 * widthPct);
		doorIconLoc.second += (int)round(40 * heightPct);
		break;
	case doorLOM:
		doorIconLoc = findIcon(doorLOMIcon);
		doorIconLoc.first += (int)round(50 * widthPct);
		doorIconLoc.second += (int)round(50 * heightPct);
		break;
	case doorDefault:
	default:
		doorIconLoc = findIcon(doorDefaultIcon);
		doorIconLoc.first += (int)round(60 * widthPct);
		doorIconLoc.second += (int)round(60 * heightPct);
		break;
	}

	return doorIconLoc;
}

pair<int, int> findStairIcon(stairType whichStair)
{
	pair<int, int> stairIconLoc;

	switch (whichStair) {
	case stairDown:
		stairIconLoc = findIcon(stairDownIcon);
		stairIconLoc.first += (int)round(60 * widthPct);
		stairIconLoc.second += (int)round(60 * heightPct);
		break;
	case stairLOM:
		stairIconLoc = findIcon(stairLOMIcon);
		stairIconLoc.first += (int)round(40 * widthPct);
		stairIconLoc.second += (int)round(40 * heightPct);
		break;
	case stairDefault:
	default:
		stairIconLoc = findIcon(stairDefaultIcon);
		stairIconLoc.first += (int)round(60 * widthPct);
		stairIconLoc.second += (int)round(60 * heightPct);
		break;
	}

	return stairIconLoc;
}

void drag(directionInfo botDirection, int slideDistance, int xStart, int yStart)
{
	xStart = (int) round(xStart * widthPct);
	yStart = (int) round(yStart * heightPct);
	int delta = 0;
	int deviation = 3;

	switch (botDirection)
	{
	case RIGHT:
		deviation = (int) round(deviation * heightPct);
		slideDistance = (int) round(slideDistance * widthPct);
		xStart = (boolRand(rng) ? xStart + slideLClick(rng) : xStart - slideLClick(rng));
		yStart = (boolRand(rng) ? yStart + slideLClick(rng) : yStart - slideLClick(rng));
		SendMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(xStart, yStart));
		sleepR(10);

		for (int i = 0; i < slideDistance; ++i)
		{
			if (boolRand(rng) && delta < deviation)
				delta += boolRand(rng);
			else if (delta > -deviation)
				delta -= boolRand(rng);

			SendMessage(window, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(xStart - i, yStart + delta));
			rSlideSleep(2);
		}

		SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(xStart - (slideDistance - 1), yStart + delta));
		break;
	case LEFT:
		deviation = (int) round(deviation * heightPct);
		slideDistance = (int) round(slideDistance * widthPct);
		xStart = (boolRand(rng) ? xStart + slideLClick(rng) : xStart - slideLClick(rng));
		yStart = (boolRand(rng) ? yStart + slideLClick(rng) : yStart - slideLClick(rng));
		SendMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(xStart, yStart));
		sleepR(10);

		for (int i = 0; i < slideDistance; ++i)
		{
			if (boolRand(rng) && delta < deviation)
				delta += boolRand(rng);
			else if (delta > -deviation)
				delta -= boolRand(rng);

			SendMessage(window, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(xStart + i, yStart + delta));
			rSlideSleep(2);
		}

		SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(xStart + (slideDistance - 1), yStart + delta));
		break;
	case UP:
		deviation = (int) round(deviation * widthPct);
		slideDistance = (int) round(slideDistance * heightPct);
		xStart = (boolRand(rng) ? xStart + slideLClick(rng) : xStart - slideLClick(rng));
		yStart = (boolRand(rng) ? yStart + slideLClick(rng) : yStart - slideLClick(rng));
		SendMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(xStart, yStart));
		sleepR(10);

		for (int i = 0; i < slideDistance; ++i)
		{
			if (boolRand(rng) && delta < deviation)
				delta += boolRand(rng);
			else if (delta > -deviation)
				delta -= boolRand(rng);

			SendMessage(window, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(xStart + delta, yStart + i));
			rSlideSleep(2);
		}

		SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(xStart + delta, yStart + (slideDistance - 1)));
		break;
	case DOWN:
		deviation = (int) round(deviation * widthPct);
		slideDistance = (int) round(slideDistance * heightPct);
		xStart = (boolRand(rng) ? xStart + slideLClick(rng) : xStart - slideLClick(rng));
		yStart = (boolRand(rng) ? yStart + slideLClick(rng) : yStart - slideLClick(rng));
		SendMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(xStart, yStart));
		sleepR(10);

		for (int i = 0; i < slideDistance; ++i)
		{
			if (boolRand(rng) && delta < deviation)
				delta += boolRand(rng);
			else if (delta > -deviation)
				delta -= boolRand(rng);

			SendMessage(window, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(xStart + delta, yStart - i));
			rSlideSleep(2);
		}

		SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(xStart + delta, yStart - (slideDistance - 1)));
		break;
	}

	longSleepR(1000);
}

void dragMap(directionInfo botDirection, int slideDistance)
{
	switch (botDirection)
	{
	case RIGHT:
		drag(RIGHT, slideDistance, 1600, 300);
		break;
	case LEFT:
		drag(LEFT, slideDistance, 185, 300);
		break;
	case UP:
		drag(UP, slideDistance, 850, 350);
		break;
	case DOWN:
		drag(DOWN, slideDistance, 850, 850);
		break;
	}
}

void Walk(directionInfo botDirection, int time, int sleepTime = 3000)
{
	int delta = 0;
	int slideDistance;
	int deviation = 3;
	int xStart = (boolRand(rng) ? (xCenter + slideLClick(rng)) : (xCenter - slideLClick(rng)));
	int yStart = (boolRand(rng) ? (yCenter + slideLClick(rng)) : (yCenter - slideLClick(rng)));

	//Start walking left
	SendMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(xStart, yStart));
	Sleep(10);

	switch (botDirection)
	{
	case LEFT:
		deviation = (int) round(deviation * heightPct);
		slideDistance = (boolRand(rng) ? slideDistanceRand(rng) : (-1) * slideDistanceRand(rng)) + 300;
		slideDistance = (int) round(slideDistance * widthPct);
		for (int i = 0; i < slideDistance; ++i)
		{
			if (boolRand(rng) && delta < 3)
				delta += boolRand(rng);
			else if (delta > -3)
				delta -= boolRand(rng);

			SendMessage(window, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(xStart - i, yStart + delta));
			Sleep(1);
		}

		Sleep(time);
		SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(xStart - (slideDistance - 1), yStart + delta));
		break;

	case RIGHT:
		deviation = (int) round(deviation * heightPct);
		slideDistance = (boolRand(rng) ? slideDistanceRand(rng) : (-1) * slideDistanceRand(rng)) + 300;
		slideDistance = (int) round(slideDistance * widthPct);
		for (int i = 0; i < slideDistance; ++i)
		{
			if (boolRand(rng) && delta < 3)
				delta += boolRand(rng);
			else if (delta > -3)
				delta -= boolRand(rng);

			SendMessage(window, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(xStart + i, yStart + delta));
			Sleep(1);
		}

		Sleep(time);
		SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(xStart + (slideDistance - 1), yStart + delta));
		break;

	case DOWN:
		deviation = (int) round(deviation * widthPct);
		slideDistance = (boolRand(rng) ? slideDistanceRand(rng) : (-1) * slideDistanceRand(rng)) + 300;
		slideDistance = (int) round(slideDistance * heightPct);
		for (int i = 0; i < slideDistance; ++i)
		{
			if (boolRand(rng) && delta < 3)
				delta += boolRand(rng);
			else if (delta > -3)
				delta -= boolRand(rng);

			SendMessage(window, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(xStart + delta, yStart + i));
			Sleep(1);
		}

		Sleep(time);
		SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(xStart + delta, yStart + (slideDistance - 1)));
		break;

	case UP:
		deviation = (int) round(deviation * widthPct);
		slideDistance = (boolRand(rng) ? slideDistanceRand(rng) : (-1) * slideDistanceRand(rng)) + 300;
		slideDistance = (int) round(slideDistance * heightPct);
		for (int i = 0; i < slideDistance; ++i)
		{
			if (boolRand(rng) && delta < 3)
				delta += boolRand(rng);
			else if (delta > -3)
				delta -= boolRand(rng);

			SendMessage(window, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(xStart + delta, yStart - i));
			Sleep(1);
		}

		Sleep(time);
		SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(xStart + delta, yStart - (slideDistance - 1)));
		break;

	}

	longSleepR(sleepTime);
}

void clickAttack(int time = 3000)
{
	leftClick(m_Button_Attack, time);
}

string getStatus()
{
	bitBltWholeScreen();
	cv::Mat partialPic;
	copyPartialPic(partialPic, 1242, 71, 257, 80);
	return getText(partialPic);
}


bool inBattle()
{
	Mat partialPic;
	bitBltWholeScreen();
	copyPartialPic(partialPic, 106, 39, 77, 37);
	return getText(partialPic).compare("Status") == 0;
}

bool endBattle()
{
	Mat partialPic1, partialPic2;
	bitBltWholeScreen();

	copyPartialPic(partialPic1, 150, 70, 20, 40);
	bool endBattleResult1 = (getText(partialPic1).compare("Items") == 0);

	copyPartialPic(partialPic2, 400, 70, 660, 80);
	bool endBattleResult2 = (getText(partialPic2).find("Got") != string::npos);

	return (endBattleResult1 || endBattleResult2);
}

int engageMobFightNow(int horrorThreshold = 7000)
{
	struct tm timeinfo;
	char buf[80], lastbuf[80];
	time_t rawtime, lastrawtime;

	int num_loop = (int)m_skillsMobSet.size();
	int iRun = 0;
	vector<int> lastSkillsRow = {0, 0, 0, 0};
	bool isThisPSlime = false;

	sleepR(1000);

	if (!inBattle()) // not a fight
	{
		return status_NotFight;
	}

	if (m_IsPrint) captureScreenNow("Mob");

	if (m_Fight_GrindingCount)
	{
		if (m_IsDebug_Fighting) cout << "Counter [" << currentGrindingCounter << "]: ";
		currentGrindingCounter++;
	}

	if (runState == grindingLOMSlimeState)
	{
		//check 578, 403, 150x150

		double MSD;
		//Check for battle
		//For 3 seconds, read the screen and compare it to the current monster pics. If a cl1ose enough similarity is found, assume its a regular battle and proceed to auto attack
		//If not, assume its a horror and exit
		Mat partialHorrorPic;
		auto horrorStartTime = chrono::high_resolution_clock::now();
		int lowestMSD = 999999999;
		while (std::chrono::duration_cast<std::chrono::seconds>(chrono::high_resolution_clock::now() - horrorStartTime).count() < 3)
		{
			bitBltWholeScreen();
			copyPartialPic(partialHorrorPic, 150, 150, 578, 403);

			for (auto i = 0; i < m_MonsterVec_LOMPSlime.size(); i++)
			{
				MSD = cv::norm(m_MonsterVec_LOMPSlime[i], partialHorrorPic);
				MSD = MSD * MSD / m_MonsterVec_LOMPSlime[i].total();

				if (MSD < lowestMSD)
				{
					lowestMSD = (int)MSD;
				}
				Sleep(10);
			}
		}

		if (lowestMSD < horrorThreshold) //If its not a monster, it must be a horror
		{
			isThisPSlime = true;

			rawtime = time(NULL);
			localtime_s(&timeinfo, &rawtime);
			strftime(buf, sizeof(lastbuf), "%Y-%m-%d_%H-%M-%S", &timeinfo);

			if (m_IsDebug_LOM) cout << "Plantium Slime encountered at " << currentLocation << " " << lowestMSD << " " << buf << endl;
		}
	}

	do
	{
		if (iRun == num_loop && m_Fight_GrindingCount)
		{
			if (m_IsDebug_Fighting) cout << "Counter [" << currentGrindingCounter << "]: ";
			currentGrindingCounter++;
		}

		iRun = iRun % num_loop;


		for (auto j = 0; j < m_CharacterFrontline; j++)
		{
			int iSkill = m_skillsMobSet[iRun][j];
			if (iSkill > m_Skill_AF)
				iSkill = 0;

			if (iSkill == m_Skill_AF)
			{
				break;
			}

			if (m_IsDebug_Fighting) cout << "C" << j << " " << iSkill << "; ";

			if (iSkill != m_Skill_Normal || iSkill != lastSkillsRow[j])
			{
				lastSkillsRow[j] = iSkill;

				leftClick(m_Button_Characters[j], 200);
				if (iSkill == m_Skill_Exchange_A || iSkill == m_Skill_Exchange_B)
				{
					leftClick(m_Button_Skills[m_Skill_Exchange], 200);
					leftClick(m_Button_Characters[iSkill - m_Skill_Exchange + m_CharacterFrontline], 200);
				}
				else
					leftClick(m_Button_Skills[iSkill], 200);

				// Click normal skill in case the skill is blocked
				leftClick(m_Button_Skills[0], 200);
				// Click front buttom in case someone is defeated
				leftClick(m_Button_Skills[4], 200);
				// Click somewhere else in case someone is disabled
				leftClick(m_Button_Yes, 200);
			}
		}

		if (m_IsDebug_Fighting) cout << endl;

		clickAttack(500);

		lastrawtime = time(NULL);
		localtime_s(&timeinfo, &lastrawtime);
		strftime(lastbuf, sizeof(lastbuf), "%Y-%m-%d_%H-%M-%S", &timeinfo);

		while (!inBattle())
		{
			rawtime = time(NULL);
			localtime_s(&timeinfo, &rawtime);
			strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);

			auto timegap = difftime(rawtime, lastrawtime);

			if (timegap > m_Fight_Timeout) // return if timeout
			{
				if (m_IsDebug_Fighting) cout << "Leaving mob fight " << lastbuf << " - " << buf << endl;
				break;
			}

			if (endBattle())
			{
				break;
			}
			Sleep(500);
		}

		iRun++;
	} while (inBattle());

	Sleep(1000);
	clickAttack(); //Get past the results screen

	if (runState == grindingLOMSlimeState)
	{
		clickAttack(1000); //Get past the results screen
	}
	return status_NoError;
}

int fightUntilEnd()
{
	int resValue = engageMobFightNow();
	return resValue;
}

int WalkUntilBattle(directionInfo botdirection = LR)
{
	int resValue;
	struct tm timeinfo;
	char buf[80], lastbuf[80];
	time_t rawtime, lastrawtime;

	lastrawtime = time(NULL);
	localtime_s(&timeinfo, &lastrawtime);
	strftime(lastbuf, sizeof(lastbuf), "%Y-%m-%d_%H-%M-%S", &timeinfo);

	while (!inBattle())
	{
		rawtime = time(NULL);
		localtime_s(&timeinfo, &rawtime);
		strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);

		auto timegap = difftime(rawtime, lastrawtime);

		if (m_IsDebug_Path) cout << "WalkUntilBattle[" << botdirection << "] " << timegap << " " << buf << endl;
		if (timegap > m_Fight_Timeout) // return if timeout
		{
			if (m_IsDebug_Path) cout << "Leaving WalkUntilBattle " << lastbuf << " - " << buf << endl;
			if (m_IsPrint) captureScreenNow("WalkUntilBattle");
			return status_Timeout;
		}

		switch (botdirection)
		{
		case LEFT:
			Walk(LEFT, m_Fight_GrindingStep);
			break;
		case RIGHT:
			Walk(RIGHT, m_Fight_GrindingStep);
			break;
		case LR:
		default:
			Walk(LEFT, m_Fight_GrindingStep);
			Walk(RIGHT, m_Fight_GrindingStep);
			break;
		}
	}

	resValue = fightUntilEnd();
	return resValue;
}

int engageHorrorFightNow(bool restoreHPMP=true)
{
	cv::Mat partialPic;
	string textToCheck;

	struct tm timeinfo;
	char buf[80], lastbuf[80];
	time_t rawtime, lastrawtime;

	sleepR(1000);

	if (!inBattle()) // not horror
	{
		leftClick(xCenter, yCenter, 3000);
		return status_NotFight;
	}

	while (!inBattle())
	{
		Walk(LEFT, 5000, 200);
		Walk(RIGHT, 5000, 200);
	}

	int num_loop = (int)m_skillsHorrorSet.size();

	rawtime = time(NULL);
	localtime_s(&timeinfo, &rawtime);
	strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);

	Mat imagePicCrop;

	if (m_IsPrint) captureScreenNow("Horror");

	int iRun = 0;
	bool isAFon = false;
	do
	{
		if (m_IsDebug_Fighting) cout << "Turn [" << iRun << "]: ";

		bitBltWholeScreen();

		// check whether the fight failed
		copyPartialPic(partialPic, 770, 90, 490, 420);
		textToCheck = getText(partialPic);
		if (textToCheck.find("Stones") != string::npos) // battle failed
		{
			if (m_IsDebug_Fighting) cout << "Fight result: " << textToCheck << endl;

			leftClick(690, 740); // Click Quit

			bitBltWholeScreen();
			copyPartialPic(partialPic, 1000, 160, 380, 360);
			textToCheck = getText(partialPic);
			if (textToCheck.find("location") != string::npos)
			{
				if (m_IsDebug_Fighting) cout << "Fight failed! " << textToCheck << endl;

				leftClick(1080, 600); // Click Quit
				sleepR(7000);
				return status_FightFail;
			}
		}

		// check whether AF bar is full
		copyPartialPic(imagePicCrop, 730, 50, 880, 30); // AF bar 880, 30, 730, 50.  AF bar 860, 10, 770, 90
		double MSD1;
		MSD1 = cv::norm(afBarFullPic, imagePicCrop);
		MSD1 = MSD1 * MSD1 / afBarFullPic.total();

		if (MSD1 < m_Fight_AFFullThreshold || isAFon)
		{
			rawtime = time(NULL);
			localtime_s(&timeinfo, &rawtime);
			strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);
			if (m_IsDebug_Fighting) cout << "Trigger AF ... " << buf << endl;
			leftClick(m_Button_AF, m_Fight_AFInterval);

			double MSD2 = 10000;
			do
			{
				leftClick(m_Button_Skills[m_Skill_for_AF], m_Fight_AFInterval);

				bitBltWholeScreen();
				copyPartialPic(imagePicCrop, 730, 50, 880, 30); // AF bar 880, 30, 730, 50.  AF bar 860, 10, 770, 90
				MSD2 = cv::norm(afBarEmptyPic, imagePicCrop);
				MSD2 = MSD2 * MSD2 / afBarEmptyPic.total();
			} while (MSD2 > 400);

			rawtime = time(NULL);
			localtime_s(&timeinfo, &rawtime);
			strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);
			if (m_IsDebug_Fighting) cout << "Trigger AF done " << buf << endl;

			if (m_IsPrint) captureScreenNow("AFDone");

			iRun = 0;
			isAFon = false;
		}
		else 
		{
			iRun = iRun % num_loop;

			for (auto j = 0; j < m_CharacterFrontline; j++)
			{
				int iSkill = m_skillsHorrorSet[iRun][j];
				if (iSkill > m_Skill_AF)
					iSkill = 0;

				if (iSkill == m_Skill_AF)
				{
					isAFon = true;
					break;
				}

				if (m_IsDebug_Fighting) cout << "C" << j << " " << iSkill << "; ";

				leftClick(m_Button_Characters[j], 200);
				if (iSkill == m_Skill_Exchange_A || iSkill == m_Skill_Exchange_B)
				{
					leftClick(m_Button_Skills[m_Skill_Exchange], 200);
					leftClick(m_Button_Characters[iSkill - m_Skill_Exchange + m_CharacterFrontline], 200);
				}
				else
					leftClick(m_Button_Skills[iSkill], 200);

				// Click normal skill in case the skill is blocked
				leftClick(m_Button_Skills[0], 200);
				// Click front buttom in case someone is defeated
				leftClick(m_Button_Skills[4], 200);
				// Click somewhere else in case someone is disabled
				leftClick(m_Button_Yes, 200);
			}

			if (m_IsDebug_Fighting) cout << endl;

			if (isAFon) 
			{
				continue;
			}

			clickAttack(500); // Fight Horror
			iRun++;
		}

		lastrawtime = time(NULL);
		localtime_s(&timeinfo, &lastrawtime);
		strftime(lastbuf, sizeof(lastbuf), "%Y-%m-%d_%H-%M-%S", &timeinfo);

		while (!inBattle())
		{
			rawtime = time(NULL);
			localtime_s(&timeinfo, &rawtime);
			strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);

			auto timegap = difftime(rawtime, lastrawtime);

			if (timegap > m_Fight_Timeout) // return if timeout
			{
				if (m_IsDebug_Fighting) cout << "Leaving horror fight " << lastbuf << " - " << buf << endl;
				break;
			}

			if (endBattle())
			{
				break;
			}

			Sleep(500);
		}

	} while (inBattle());

	rawtime = time(NULL);
	localtime_s(&timeinfo, &rawtime);
	strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);
	if (m_IsDebug_Fighting) cout << "Battle ends " << currentLocation << " " << buf << endl;

	if (m_IsPrint) captureScreenNow("Result");

	/*
	bitBltWholeScreen();
	copyPartialPic(partialPic, 770, 90, 490, 420);
	copyPartialPic(imagePicCrop, M_WIDTH, M_HEIGHT, 0, 0);
	textToCheck = getText(partialPic);

	if (textToCheck.find("Stones") != string::npos) // battle failed
	{
		leftClick(690, 740); // Click Quit

		bitBltWholeScreen();
		copyPartialPic(partialPic, 1000, 160, 380, 360);
		textToCheck = getText(partialPic);

		if (textToCheck.find("location") != string::npos)
		{
			leftClick(1080, 600); // Click Quit
			sleepR(7000);
			return 1; // failed
		}
	}
	*/
	longSleepR(4000);

	clickAttack(); //Get past the results screen
	clickAttack(); //Get past the fish results screen

	longSleepR(4000);

	if (restoreHPMP)
	{
		// restore HP & MP
		leftClick(m_Button_Menu);
		leftClick(m_Button_Food);
		leftClick(m_Button_Yes, 4000);
		leftClick(m_Button_Yes);
		leftClick(m_Button_Yes);
		leftClick(m_Button_Yes);
	}

	return status_NoError;
}

//Strategy is to quadrisect the lake and toss into each of the four sections and center
void fish(vector<pair<int, int>>& sections, int msdThreshold = 10000, int horrorThreshold = 7000)
{
	double catchIndex = 0, horrorIndex = 0, monsterIndex = 0;
	struct tm timeinfo;
	char buf[80];
	time_t rawtime, lastCatch;

	rawtime = time(NULL);
	localtime_s(&timeinfo, &rawtime);
	strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);
	lastCatch = rawtime;

	if (m_IsDebug_Fishing) cout << "Starting fishing at " << currentLocation << " " << buf << endl;

	longSleepR(1000);

	vector<pair<int, int>> baits = { {827, 274}, {882, 397}, {870, 514}, {851, 630}, {854, 750}, {884, 790} };

	auto changeBait = [&baits](baitType type)
	{
		int slotNum = 0;
		if (type > 0)
		{
			//Calculate slot that bait is in
			for (int i = 0; i < type; ++i)
			{
				if (m_baitList[i].first == true)
					++slotNum;
			}
		}

		leftClick(1225, 925);

		if (slotNum > 4)
		{
			drag(DOWN, 74, xCenter, yCenter);
			for (int i = 5; i < slotNum; ++i)
			{
				drag(DOWN, 100, xCenter, yCenter);
			}

			leftClick(baits[5].first, baits[5].second);
		}
		else
			leftClick(baits[slotNum].first, baits[slotNum].second);
	};

	bitBltWholeScreen();
	Mat lakeImg = bitbltPic.clone();

	for (int i = 0; i < currentBaitsToUse->size(); ++i)
	{
		if (m_baitList[(*currentBaitsToUse)[i]].first == false) //If we have 0 of current bait, go to next bait
			continue;

		changeBait((*currentBaitsToUse)[i]);

		shuffle(sections.begin() + 1, sections.end(), default_random_engine((unsigned int) std::chrono::system_clock::now().time_since_epoch().count()));

		bool increaseSection = false;
		int lastSection = 0;
		for (int j = 0; j < sections.size(); )
		{
			bitBltWholeScreen();
			cv::Mat partialPic;
			copyPartialPic(partialPic, 286, 55, 1070, 893);  // 995 - 785 = 210, 695 - 655 = 40, 785, 655, 
			string textChangeBait = getText(partialPic);
			if (textChangeBait.find("Change") == string::npos) //No Change Bait
			{
				if (m_IsDebug_Fishing) cout << textChangeBait << endl;

				if (m_IsPrint) captureScreenNow("NOTFISHING");

				rawtime = time(NULL);
				localtime_s(&timeinfo, &rawtime);
				strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);
				if (m_IsDebug_Fishing) cout << "Leaving " << currentLocation << " (" << horrorIndex << "/" << catchIndex << ") catched - Wrong fishing interface " << buf << endl;
				leftClick(m_Button_X);
				return;
			}

			//Cast line
			leftClick(sections[j].first, sections[j].second);

			string status = getStatus();
			if (status.find("bait") != string::npos) //Out of bait
			{
				m_baitList[(*currentBaitsToUse)[i]].first = false;

				do //Look for a bait that we actually have
				{
					++i;
				} while (i < currentBaitsToUse->size() && m_baitList[(*currentBaitsToUse)[i]].first == false);

				if (i >= currentBaitsToUse->size())
					return;

				changeBait((*currentBaitsToUse)[i]);
				leftClick(sections[j].first, sections[j].second);
			}
			else if (status.find("any fish") != string::npos || status.find("box") != string::npos) //Pool is empty or cooler is full
			{
				rawtime = time(NULL);
				localtime_s(&timeinfo, &rawtime);
				strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);

				if (m_IsDebug_Fishing) cout << "Leaving " << currentLocation << " (" << horrorIndex << "/" << catchIndex << ") catched - No fish or no space " << buf << endl;
				return;
			}

			double MSD;
			auto startTime = chrono::high_resolution_clock::now();
			bool noFishCheck = true;

			while (1) //This constantly reads the screen looking for either a certain status, or whether a zoom in has occurred
			{
				//getStatus() is really cpu intensive, so avoid doing it unless necessary
				if (std::chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime).count() >= 11000 + longSleepRand(rng) && noFishCheck)
				{
					noFishCheck = false;
					status = getStatus();

					if (status.find("no fish") != string::npos)
					{
						increaseSection = true;
						leftClick(sections[j].first, sections[j].second);
						break;
					}
				}

				bitBltWholeScreen();
				MSD = cv::norm(lakeImg, bitbltPic);
				MSD = MSD * MSD / lakeImg.total();

				if (MSD > msdThreshold) //If the current screen is sufficiently different (high mean square difference) from the normal lake image, then a zoom in has occurred
				{
					Sleep(100 + sleepRand(rng)); //Emulate human reaction time
					leftClick(sections[j].first, sections[j].second);

					catchIndex++;

					//Click through success or failure
					longSleepR(4000);
					leftClick(sections[j].first, sections[j].second);
					longSleepR(2000);

					bitBltWholeScreen();
					MSD = cv::norm(lakeImg, bitbltPic);
					MSD = MSD * MSD / lakeImg.total();
					if (MSD > msdThreshold) //Double, gotta click past it
					{
						leftClick(sections[j].first, sections[j].second);
						longSleepR(2000);

						bitBltWholeScreen();
						MSD = cv::norm(lakeImg, bitbltPic);
						MSD = MSD * MSD / lakeImg.total();
						if (MSD > msdThreshold) //Triple
						{
							leftClick(sections[j].first, sections[j].second);
							longSleepR(2000);
						}
					}

					//Check for battle
					bitBltWholeScreen();
					MSD = cv::norm(lakeImg, bitbltPic);
					MSD = MSD * MSD / lakeImg.total();
					if (MSD > msdThreshold) //Should have returned to normal lake image; if not, its a battle
					{
						Sleep(5000); //Give ample time for battle to fully load

						if (inBattle()) // double check whether it is a battle
						{
							if (hasHorror) //Its possibly a horror or lakelord, so need to check to make sure before trying to auto it down
							{
								bool isThisHorror = false;
								if (currentMonsterVec != NULL)
								{
									//For 3 seconds, read the screen and compare it to the current monster pics. If a cl1ose enough similarity is found, assume its a regular battle and proceed to auto attack
									//If not, assume its a horror and exit
									Mat partialHorrorPic;
									auto horrorStartTime = chrono::high_resolution_clock::now();
									int lowestMSD = 999999999;
									while (std::chrono::duration_cast<std::chrono::seconds>(chrono::high_resolution_clock::now() - horrorStartTime).count() < 3)
									{
										bitBltWholeScreen();
										copyPartialPic(partialHorrorPic, 140, 140, 420, 380);
										for (int i = 0; i < currentMonsterVec->size(); ++i)
										{
											MSD = cv::norm((*currentMonsterVec)[i], partialHorrorPic);
											MSD = MSD * MSD / (*currentMonsterVec)[i].total();

											if (MSD < lowestMSD)
											{
												lowestMSD = (int)MSD;
												monsterIndex = i;
											}
										}
										Sleep(10);
									}

									if (lowestMSD > horrorThreshold) //If its not a monster, it must be a horror
										isThisHorror = true;
								}
								else
								{
									isThisHorror = true;
								}

								if (isThisHorror)
								{
									rawtime = time(NULL);
									localtime_s(&timeinfo, &rawtime);
									strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);

									if (m_IsDebug_Fishing) cout << "Horror or Lake Lord " << currentLocation << " detected " << horrorIndex << " " << buf << endl;
									if (isFightHorror)
									{
										int fightResult = engageHorrorFightNow();
										horrorIndex++;
										
										if (fightResult != status_NoError) // failed
										{
											if (m_IsDebug_Fishing) cout << "Fight Horror or Lake Lord failed!" << endl;
											return;
										}
										if (horrorIndex >= m_Fight_HorrorCount) // limit the number of horrors / lake lords to fight
										{
											if (m_IsDebug_Fishing) cout << "Leaving " << currentLocation << " (" << horrorIndex << "/" << catchIndex << ") catched - Horror count reached " << buf << endl;
											return;
										}
									}
									else
										exit(0);
								}
								else
								{
									if (m_IsDebug_Fishing) cout << "Fighting " << currentLocation << " monster (" << monsterIndex << ")" << endl;
								}
							}

							fightUntilEnd();
							clickAttack(); //Click past fish results screen
							Sleep(5000);

							if (fishIconClickFunction != NULL) //In Man Eating Swamp, any battle shifts your position, so the fish icon location must be found again
							{
								fishIconClickFunction();
							}
							else
							{
								leftClick(currentFishIconLoc.first, currentFishIconLoc.second, 2000);
							}

							longSleepR(1000);
						} 
						else // not a battle, probably a fish
						{
							leftClick(sections[j].first, sections[j].second);
							longSleepR(2000);
						}
					}

					rawtime = time(NULL);
					localtime_s(&timeinfo, &rawtime);
					lastCatch = rawtime;
					lastSection = j;
					break;

				}
				else
				{
					// if wait too long, let's move to other area.
					rawtime = time(NULL);
					localtime_s(&timeinfo, &rawtime);
					strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);

					auto timegap = difftime(rawtime, lastCatch);

					if (timegap > 300)
					{
						if (m_IsDebug_Fishing) cout << "Leaving " << currentLocation << " (" << horrorIndex << "/" << catchIndex << ") catched - Idling " << buf << endl;
						return;
					}
				}

				Sleep(12); //Try to stay around 60 fps read time
			}

			if (increaseSection)
			{
				++j;
				j %= sections.size();
				if (j == lastSection)
					break;
			}

			increaseSection = false;
		}
	}

	rawtime = time(NULL);
	localtime_s(&timeinfo, &rawtime);
	strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);

	if (m_IsDebug_Fishing) cout << "Leaving " << currentLocation << " (" << horrorIndex << "/" << catchIndex << ") catched - All baits tried " << buf << endl;
}

int goToTargetLocation(vector<pathInfo> pathInfoList)
{
	struct tm timeinfo;
	char buf[80];
	time_t rawtime;

	rawtime = time(NULL);
	localtime_s(&timeinfo, &rawtime);
	strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);

	if (m_IsDebug_Path) cout << "----> " << currentLocation << " " << buf << endl;

	for (auto j = 0; j < pathInfoList.size(); j++)
	{
		string curType = pathInfoList[j].type;
		string curValue1 = pathInfoList[j].value1;
		string curValue2 = pathInfoList[j].value2;

		if (m_IsDebug_Path) cout << curType << " " << curValue1 << " " << curValue2 << endl;

		if (curType.compare("Click") == 0)
		{
			if (!curValue2.empty())
			{
				int x = stoi(curValue1);
				int y = stoi(curValue2);

				x = (int)round(x * M_WIDTH / M_WIDTH_1280);
				y = (int) round(y * M_HEIGHT / M_HEIGHT_720);

				leftClick(x, y);
			}
			else
			{
				if (curValue1.compare("YesButton") == 0)
				{
					leftClick(m_Button_Yes);
				}
				else if (curValue1.compare("MapButton") == 0)
				{
					leftClick(m_Button_Map, 3500);
				}
				else if (curValue1.compare("Antiquity") == 0)
				{
					leftClick(m_Button_Antiquity);
				}
				else if (curValue1.compare("Present") == 0)
				{
					leftClick(m_Button_Present);
				}
				else if (curValue1.compare("Future") == 0)
				{
					leftClick(m_Button_Future);
				}
				else if (curValue1.compare("EndOfTimeLoc") == 0)
				{
					leftClick(m_Button_EndOfTimeLoc);
				}
				else
				{
					bool isfound = false;

					for (auto i = 0; i < m_Button_MapButtons.size(); i++)
					{
						if (curValue1.compare(m_Button_MapButtons[i].buttonName) == 0)
						{
							isfound = true;

							leftClick(m_Button_MapButtons[i].xyPosition);
							break;
						}
					}

					if (m_IsDebug_Path) if (!isfound) cout << "!!! " << curValue1 << " not found" << endl;
				}
			}
		}
		else if (curType.compare("Sleep") == 0) {

			if (curValue1.compare("LoadTime") == 0)
			{
				longSleepR(loadTime);
			}
			else
			{
				longSleepR(stoi(curValue1));
			}
		}
		else if (curType.compare("Walk") == 0) {

			if (curValue1.compare("LEFT") == 0)
			{
				Walk(LEFT, stoi(curValue2));
			}
			else if (curValue1.compare("RIGHT") == 0)
			{
				Walk(RIGHT, stoi(curValue2));
			}
			else if (curValue1.compare("DOWN") == 0)
			{
				Walk(DOWN, stoi(curValue2));
			}
			else if (curValue1.compare("UP") == 0)
			{
				Walk(UP, stoi(curValue2));
			}
		}
		else if (curType.compare("Drag") == 0) {
			if (curValue1.compare("LEFT") == 0)
			{
				dragMap(LEFT, stoi(curValue2));
			}
			else if (curValue1.compare("RIGHT") == 0)
			{
				dragMap(RIGHT, stoi(curValue2));
			}
			else if (curValue1.compare("DOWN") == 0)
			{
				dragMap(DOWN, stoi(curValue2));
			}
			else if (curValue1.compare("UP") == 0)
			{
				dragMap(UP, stoi(curValue2));
			}
		}
		else if (curType.compare("Find") == 0) {
			if (curValue1.compare("Exclamation") == 0)
			{
				pair<int, int> pnt = findExclamationIcon();
				leftClick(pnt.first, pnt.second, 2000, false);
			}
			if (curValue1.compare("ExclamationRift") == 0)
			{
				pair<int, int> pnt = findExclamationIcon(exclRift);
				leftClick(pnt.first, pnt.second, 2000, false);
			}
			if (curValue1.compare("ExclamationSpecter") == 0)
			{
				pair<int, int> pnt = findExclamationIcon(exclSepcter);
				leftClick(pnt.first, pnt.second, 2000, false);
			}
			if (curValue1.compare("ExclamationGrasta") == 0)
			{
				pair<int, int> pnt = findExclamationIcon(exclGrasta);
				leftClick(pnt.first, pnt.second, 2000, false);
			}
			if (curValue1.compare("ExclamationChamber") == 0)
			{
				pair<int, int> pnt = findExclamationIcon(exclChamber);
				leftClick(pnt.first, pnt.second, 2000, false);
			}
			if (curValue1.compare("ExclamationChamberPlasma") == 0)
			{
				pair<int, int> pnt = findExclamationIcon(exclChamberPlasma);
				leftClick(pnt.first, pnt.second, 2000, false);
			}
			if (curValue1.compare("ExclamationKMS") == 0)
			{
				pair<int, int> pnt = findExclamationIcon(exclKMS);
				leftClick(pnt.first, pnt.second, 2000, false);
			}
			else if (curValue1.compare("Door") == 0)
			{
				pair<int, int> pnt = findDoorIcon(doorDefault);
				leftClick(pnt.first, pnt.second, 2000, false);
			}
			else if (curValue1.compare("DoorKunlun") == 0)
			{
				pair<int, int> pnt = findDoorIcon(doorKunlun);
				leftClick(pnt.first, pnt.second, 2000, false);
			}
			else if (curValue1.compare("DoorLOM") == 0)
			{
				pair<int, int> pnt = findDoorIcon(doorLOM);
				leftClick(pnt.first, pnt.second, 2000, false);
			}
			else if (curValue1.compare("Stair") == 0)
			{
				pair<int, int> pnt = findStairIcon(stairDefault);
				leftClick(pnt.first, pnt.second, 2000, false);
			}
			else if (curValue1.compare("StairDown") == 0)
			{
				pair<int, int> pnt = findStairIcon(stairDown);
				leftClick(pnt.first, pnt.second, 2000, false);
			}
			else if (curValue1.compare("StairLOM") == 0)
			{
				pair<int, int> pnt = findStairIcon(stairLOM);
				leftClick(pnt.first, pnt.second, 2000, false);
			}
			else if (curValue1.compare("SwampFishIcon") == 0)
			{
				findAndClickSwampFishIcon();
			}
			else if (curValue1.compare("FishIcon") == 0)
			{
				findAndClickFishIcon();
			}
			else // dynamic image
			{
				for (auto k = 0; k < m_DynamicImage.size(); k++)
				{
					if (curValue1.compare(m_DynamicImage[k].name) == 0)
					{
						pair<int, int> iconLoc = findIcon(m_DynamicImage[k].image);
						iconLoc.first += (int)round(m_DynamicImage[k].width * widthPct);
						iconLoc.second += (int)round(m_DynamicImage[k].width * heightPct);
						leftClick(iconLoc.first, iconLoc.second, 2000, false);
						break;
					}
				}
			}
		}
		else if (curType.compare("Fight") == 0) {
			if (curValue1.compare("UntilEnd") == 0)
			{
				fightUntilEnd();
			}
			else if (curValue1.compare("UntilBattle") == 0)
			{
				int resValue;
				if (curValue2.empty())
					resValue = WalkUntilBattle();
				else if (curValue2.compare("LEFT") == 0)
					resValue = WalkUntilBattle(LEFT);
				else if (curValue2.compare("RIGHT") == 0)
					resValue = WalkUntilBattle(RIGHT);
				else
					resValue = WalkUntilBattle();

				if (resValue == status_Timeout)
					return resValue;
			}
		}
		else if (curType.compare("Capture") == 0) {
			if (curValue1.compare("Screen") == 0)
			{
				captureScreenNow();
			}
		}
	}

	return status_NoError;
}

void goToFishingLocation()
{
	for (int i = 0; i < m_LocationList.size(); i++)
	{
		if (m_LocationList[i].locationName.compare(currentLocation) == 0)
		{
			goToTargetLocation(m_LocationList[i].pathStepsList);
		}
	}
}

void goToSpacetimeRift(bool heal = true)
{
	if (m_IsDebug_Path) cout << "Spacetime Rift [" << heal << "]" << endl;

	leftClick(m_Button_Map, 3500);
	leftClick(m_Button_EndOfTimeLoc);
	leftClick(m_Button_SpacetimeRift);
	leftClick(m_Button_SpacetimeRift);
	leftClick(m_Button_Yes);
	longSleepR(loadTime);

	if (heal)
	{
		leftClick(m_Button_Tree);
		leftClick(m_Button_Tree);
		leftClick(m_Button_Yes, 4000);
		leftClick(m_Button_Yes);
		leftClick(m_Button_Yes);
		leftClick(m_Button_Yes);
	}
}

void goToFishVendor()
{
	auto getVendorText = [](int columns, int rows, int x, int y)
	{
		bitBltWholeScreen();
		cv::Mat partialPic;
		copyPartialPic(partialPic, columns, rows, x, y);
		return getText(partialPic);
	};

	auto getVendorNumber = [](int columns, int rows, int x, int y)
	{
		bitBltWholeScreen();
		cv::Mat partialPic;
		copyPartialPic(partialPic, columns, rows, x, y);
		return getNumber(partialPic);
	};

	goToSpacetimeRift(false);
	leftClick(m_Button_Map, 3500);
	leftClick(m_Button_Antiquity);

	for (auto i = 0; i < m_Button_MapButtons.size(); i++)
	{
		if (m_Button_MapButtons[i].buttonName.compare("KiraBeach") == 0)
		{ 
			leftClick(m_Button_MapButtons[i].xyPosition);
			break;
		}
	}

	leftClick(m_Button_Yes);
	longSleepR(loadTime);
	Walk(LEFT, 1000);
	longSleepR(1000);

	pair<int, int> pnt = findExclamationIcon();
	leftClick(pnt.first, pnt.second, 2000, false);

	for (int i = 0; i < 30; ++i) //click through text
		leftClick(380, 355, 100);

	leftClick(878, 888);

	//Buy baits if needed
	const static vector<pair<int, int>> baitsLocs = { {1220, 160}, {1215, 325}, {1215, 465}, {1215, 625}, {1215, 800}, {1225, 925} };
	const static vector<int> baitCosts = { 500, 1000, 3000, 1, 2, 3, 4, 6, 10, 30, 4000, 6000 };
	string currText;
	const static vector<string> baitNames = { "Fishing Dango", "Worm", "Unexpected Worm", "Shopaholic Clam", "Spree Snail", "Dressed Crab", "Tear Crab", "Foamy Worm", "Bubbly Worm", "Snitch Sardine", "Blabber Sardine", "Crab Cake", "Premium Crab Cake" };

	//Only 5 bait slots are fully on screen at a time, with the 6th partially visible. For every bait after 6, scroll down until new bait occupies bottom slot and then attempt to buy
	for (int i = 0; i < m_baitList.size(); ++i)
	{
		if (i > 4)
		{
			if (i == 5)
				drag(DOWN, 60, 1225, 550);
			else
				drag(DOWN, m_SectionHeight, 1225, 550);
			leftClick(baitsLocs[5].first, baitsLocs[5].second);
		}
		else
			leftClick(baitsLocs[i].first, baitsLocs[i].second);

		//If the next one we've done is the same as the last, we've reached the end of the list
		string txt = getVendorText(577, 51, 100, 102);
		if (currText.compare(txt) == 0)
			break;
		currText = txt;

		while (txt.compare(baitNames[i]) != 0 && i < m_baitList.size()) //You don't get the baits in order, so some may be missing depending on your progression (blabber sardines)
			++i;


		//Get amount currently held
		int held = getVendorNumber(141, 30, 500, 165);
		int numToBuy = m_baitList[i].second - held;

		if (numToBuy > 0)
		{
			if (i > 2 && i < 10) //Fish point bait. Here, since fish points aren't infinite like gold, only buy as much as you can up to the max set in config
			{
				//Get amount of fish points owned
				int fishPoints = getVendorNumber(193, 28, 1346, 10);
				//Get cost of current selected bait

				int fpCost = baitCosts[i];
				if (fpCost < fishPoints) //Can we even afford one?
				{
					if (numToBuy * fpCost >= fishPoints) //If we don't have enough fish points to buy the max set, then buy as many as we can
						numToBuy = (int)floor(fishPoints / fpCost);
				}
			}

			int thisTimeToBuy;
			if (m_IsDebug_Fishing) cout << "Bait [" << i << "]" << m_baitList[i].second << ": holding " << held << " to buy " << numToBuy << ". buying";

			for (; numToBuy > 0; numToBuy -= thisTimeToBuy)
			{
				if (numToBuy > 99)
					thisTimeToBuy = 99;
				else 
					thisTimeToBuy = numToBuy;


				if (m_IsDebug_Fishing) cout << " " << thisTimeToBuy;

				if (thisTimeToBuy > 0)
				{
					for (int j = 0; j < thisTimeToBuy - 1; ++j)
						leftClick(555, 665, 500);

					leftClick(350, 850);
					leftClick(1070, 650);
					leftClick(880, 575);

					held = getVendorNumber(141, 30, 500, 165);
				}
			}
			if (m_IsDebug_Fishing) cout << endl;
		}

		m_baitList[i].first = held > 0;
	}

	leftClick(m_Button_X);
}

void fishing_kiraBeach()
{
	fishIconClickFunction = &findAndClickFishIcon;

	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	fish(m_Locs_KiraBeach);
	leftClick(m_Button_Leave);

	fishIconClickFunction = NULL;
}

void fishing_baruoki()
{
	hasHorror = true;
	currentMonsterVec = &m_MonsterVec_Baruoki;

	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(711, 366);

	fish(m_Locs_Baruoki);
	leftClick(m_Button_Leave);

	hasHorror = false;
	currentMonsterVec = NULL;
}

void fishing_naaruUplands()
{
	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(775, 450);

	fish(m_Locs_Baruoki);
	leftClick(m_Button_Leave);
}

void fishing_acteul()
{
	hasHorror = true;
	currentMonsterVec = &m_MonsterVec_Acteul;

	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(1029, 367);

	fish(m_Locs_Acteul);
	leftClick(m_Button_Leave);

	hasHorror = false;
	currentMonsterVec = NULL;
}

void fishing_elzionAirport()
{
	hasHorror = true;
	currentMonsterVec = &m_MonsterVec_Elzion;

	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(1004, 448);

	fish(m_Locs_Elzion);
	leftClick(m_Button_Leave);

	hasHorror = false;
	currentMonsterVec = NULL;
}

void fishing_zolPlains()
{
	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(935, 455);

	fish(m_Locs_ZolPlains);
	leftClick(m_Button_Leave);
}

void fishing_lakeTillian()
{
	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(741, 448);

	fish(m_Locs_Acteul);
	leftClick(m_Button_Leave);
}

void fishing_vasuMountain()
{
	hasHorror = true;
	currentMonsterVec = &m_MonsterVec_Vasu;

	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(727, 442);

	fish(m_Locs_Vasu);
	leftClick(m_Button_Leave);

	hasHorror = false;
	currentMonsterVec = NULL;
}

void fishing_karekSwampland()
{
	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(623, 448);

	fish(m_Locs_Acteul);
	leftClick(m_Button_Leave);
}

void fishing_moonlightForest()
{
	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(435, 360);

	fish(m_Locs_Moonlight);
	leftClick(m_Button_Leave);
}

void fishing_ancientBattlefield()
{
	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(1300, 455);

	fish(m_Locs_AncientBattlefield);
	leftClick(m_Button_Leave);
}

void fishing_snakeNeckIgoma()
{
	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(685, 450);

	fish(m_Locs_Igoma);
	leftClick(m_Button_Leave);
}

void fishing_rinde()
{
	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(970, 368);

	fish(m_Locs_KiraBeach);
	leftClick(m_Button_Leave);
}

void fishing_serenaCoast()
{
	hasHorror = true;
	currentMonsterVec = &m_MonsterVec_Serena;

	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(796, 448);

	fish(m_Locs_Acteul);
	leftClick(m_Button_Leave);

	hasHorror = false;
	currentMonsterVec = NULL;
}

void fishing_rucyanaSands()
{
	hasHorror = true;
	currentMonsterVec = &m_MonsterVec_Rucyana;

	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(744, 411);

	fish(m_Locs_RucyanaSands);
	leftClick(m_Button_Leave);

	hasHorror = false;
	currentMonsterVec = NULL;
}

void fishing_lastIsland()
{
	hasHorror = true;
	currentMonsterVec = &m_MonsterVec_LastIsland;

	fishIconClickFunction = &findAndClickFishIcon;

	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	fish(m_Locs_KiraBeach);
	leftClick(m_Button_Leave);

	fishIconClickFunction = NULL;

	hasHorror = false;
	currentMonsterVec = NULL;
}

void fishing_nilva()
{
	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(1068, 400);

	fish(m_Locs_Elzion);
	leftClick(m_Button_Leave);
}

void fishing_manEatingSwamp()
{
	fishIconClickFunction = &findAndClickSwampFishIcon;

	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	fish(m_Locs_Acteul, 5500);
	leftClick(m_Button_Leave);

	fishIconClickFunction = NULL;
}

void fishing_charolPlains()
{
	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(840, 456);

	fish(m_Locs_Baruoki);
	leftClick(m_Button_Leave);
}

void fishing_dimensionRift()
{
	hasHorror = true;
	currentMonsterVec = &m_MonsterVec_DimensionRift;

	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(408, 345);

	fish(m_Locs_DimensionRift, 10000, 2500);
	leftClick(m_Button_Leave);

	hasHorror = false;
	currentMonsterVec = NULL;
}

void fishing_outerWallPlum()
{
	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(450, 400);

	fish(m_Locs_DragonPalace);
	leftClick(m_Button_Leave);
}

void fishing_outerWallPlumPast()
{
	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(450, 400);

	fish(m_Locs_DragonPalace);
	leftClick(m_Button_Leave);
}

void fishing_outerWallBamboo()
{
	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(1240, 410);

	fish(m_Locs_DragonPalace);
	leftClick(m_Button_Leave);
}

void fishing_outerWallBambooPast()
{
	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(1240, 410);

	fish(m_Locs_DragonPalace);
	leftClick(m_Button_Leave);
}

void fishing_innerWall()
{
	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(751, 407);

	fish(m_Locs_DragonPalace);
	leftClick(m_Button_Leave);
}

void fishing_innerWallPast()
{
	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(603, 408);

	fish(m_Locs_DragonPalace);
	leftClick(m_Button_Leave);
}

void fishing_outerWallPine()
{
	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(630, 408);

	fish(m_Locs_DragonPalace);
	leftClick(m_Button_Leave);
}

void fishing_outerWallPinePast()
{
	//goToFishVendor();
	goToSpacetimeRift();
	goToFishingLocation();

	currentFishIconLoc = make_pair(630, 405);

	fish(m_Locs_DragonPalace);
	leftClick(m_Button_Leave);
}

void loadSettingConfig()
{
	//Read in config file
	string key, value;
	ifstream file("config_setting.txt");
	string str;
	set<int> baitsUsedThisRun;

	vector<int> skillsRow = vector<int>(4);

	pair<int, int> xyPosition;
	buttonInfo nameButton;

	auto ltrimString = [&str]()
	{
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
			return !std::isspace(ch);
			}));
	};

	auto getValueKey = [&key, &value](string& str)
	{
		int loc = (int)str.find("=");
		key = str.substr(loc + 1);
		value = str.substr(0, loc);
	};

	auto parseXYinfo = [&key, &xyPosition](string& str)
	{
		int loc = (int)str.find(",");

		int x = stoi(str.substr(0, loc));
		int y = stoi(str.substr(loc + 1));

		xyPosition.first = (int)round(x * M_WIDTH / M_WIDTH_1280);
		xyPosition.second = (int)round(y * M_HEIGHT / M_HEIGHT_720);
	};

	auto getSkillArrayValue = [&skillsRow](string& str)
	{
		std::string token;
		size_t pos = 0;
		size_t index = 0;
		while (index < m_CharacterFrontline) {
			pos = str.find(",");
			if (pos != std::string::npos)
				token = str.substr(0, pos);
			else
				token = str;

			skillsRow[index] = stoi(token);
			str.erase(0, pos + 1);
			index++;
		}
	};

	auto parseSkillsHorrorSet = [&ltrimString, &str, &file, &getSkillArrayValue, &skillsRow](vector<vector<int>>& skillsHorrorSet)
	{
		while (1)
		{
			std::getline(file, str);
			ltrimString();
			if (str[0] == '-' || str[0] == '\n') //skip comments and blank lines
				continue;
			if (str.compare("~END") == 0)
				break;

			getSkillArrayValue(str);
			skillsHorrorSet.push_back(skillsRow);
		}
	};

	auto parseSkillsMobSet = [&ltrimString, &str, &file, &getSkillArrayValue, &skillsRow](vector<vector<int>>& skillsMobSet)
	{
		while (1)
		{
			std::getline(file, str);
			ltrimString();
			if (str[0] == '-' || str[0] == '\n') //skip comments and blank lines
				continue;
			if (str.compare("~END") == 0)
				break;

			getSkillArrayValue(str);
			skillsMobSet.push_back(skillsRow);
		}
	};

	auto parseButtons = [&ltrimString, &str, &file, &parseXYinfo, &xyPosition](vector<pair<int, int>>& buttonlist)
	{
		while (1)
		{
			std::getline(file, str);
			ltrimString();
			if (str[0] == '-' || str[0] == '\n') //skip comments and blank lines
				continue;
			if (str.compare("~END") == 0)
				break;

			parseXYinfo(str);
			buttonlist.push_back(xyPosition);
		}
	};

	auto parseNameButtons = [&ltrimString, &str, &file, &key, &value, &getValueKey, &parseXYinfo, &xyPosition, &nameButton](vector<buttonInfo>& buttonlist)
	{
		while (1)
		{
			std::getline(file, str);
			ltrimString();
			if (str[0] == '-' || str[0] == '\n') //skip comments and blank lines
				continue;
			if (str.compare("~END") == 0)
				break;

			getValueKey(str);
			parseXYinfo(value);

			nameButton.buttonName = key;
			nameButton.xyPosition = xyPosition;

			if (m_IsDebug_Path) cout << "Button [" << key << "]: " << xyPosition.first << " " << xyPosition.second << endl;

			buttonlist.push_back(nameButton);
		}
	};

	auto parseGrindingSpotsList = [&ltrimString, &str, &file, &getValueKey, &key, &value](vector<pair<string, int>>& grindingSpots)
	{
		while (1)
		{
			std::getline(file, str);
			ltrimString();
			if (str[0] == '-' || str[0] == '\n') //skip comments and blank lines
				continue;
			if (str.compare("~END") == 0)
				break;

			getValueKey(str);
			if (stoi(value))
			{ 
				pair<string, int> grindingspot = { key, stoi(value) };
				grindingSpots.push_back(grindingspot);
			}
		}
	};

	auto parseGrastaNames = [&ltrimString, &str, &file, &key, &value, &getValueKey, &nameButton](vector<string>& grastanamelist)
	{
		while (1)
		{
			std::getline(file, str);
			ltrimString();
			if (str[0] == '-' || str[0] == '\n') //skip comments and blank lines
				continue;
			if (str.compare("~END") == 0)
				break;

			getValueKey(str);

			if (stoi(value))
			{
				if (m_IsDebug_Grasta) cout << "To separate " << key << endl;
				grastanamelist.push_back(key);
			}
		}
	};

	while (std::getline(file, str))
	{
		if (str[0] == '-' || str[0] == '\n') //skip comments and blank lines
			continue;

		getValueKey(str);

		if (key.compare("Load Time") == 0)
		{
			loadTime = stoi(value);
		}
		else if (key.compare("Debug") == 0)
		{
			int debugvalue = stol(value, NULL, 2);

			m_IsDebug = debugvalue & 0x1;
			m_IsDebug_Path = debugvalue & 0x2;
			m_IsDebug_Fighting = debugvalue & 0x4;
			m_IsDebug_Grinding = debugvalue & 0x8;
			m_IsDebug_Fishing = debugvalue & 0x10;
			m_IsDebug_Grasta = debugvalue & 0x20;
			m_IsDebug_LOM = debugvalue & 0x40;
		}
		else if (key.compare("Print Image") == 0)
		{
			m_IsPrint = stoi(value);
		}

		/******************/
		/* Skills section */
		/******************/
		else if (key.compare("Horror Count") == 0)
		{
			m_Fight_HorrorCount = stoi(value);
		}
		else if (key.compare("AF Interval") == 0)
		{
			m_Fight_AFInterval = stoi(value);
		}
		else if (key.compare("AF Full Threshold") == 0)
		{
			m_Fight_AFFullThreshold = stoi(value);
		}
		else if (key.compare("Spacetime Rift") == 0)
		{
			m_Fight_GoToSpacetimeRift = stoi(value);
		}
		else if (key.compare("Run Count") == 0)
		{
			if (value.compare("*") == 0)
			{
				m_Fight_EndlessGrinding = true;
				m_Fight_GrindingCount = 0;
			}
			else
			{
				m_Fight_EndlessGrinding = false;
				m_Fight_GrindingCount = stoi(value);
			}

		}
		else if (key.compare("Run Steps") == 0)
		{
			m_Fight_GrindingStep = stoi(value);
		}
		else if (key.compare("Direction") == 0)
		{
			if (value.compare("LEFT") == 0)
				m_Fight_GrindingDirection = LEFT;
			else if (value.compare("RIGHT") == 0)
				m_Fight_GrindingDirection = RIGHT;
			else if (value.compare("LR") == 0)
				m_Fight_GrindingDirection = LR;
		}
		else if (key.compare("Timeout") == 0)
		{
			m_Fight_Timeout = stoi(value);
		}
		else if (key.compare("SkillsHorrorSet") == 0)
		{
			m_skillsHorrorSet.clear();
			parseSkillsHorrorSet(m_skillsHorrorSet);
		}
		else if (key.compare("SkillsMobSet") == 0)
		{
			m_skillsMobSet.clear();
			parseSkillsMobSet(m_skillsMobSet);
		}
		else if (key.compare("GrindingSpots") == 0)
		{
			m_grindingSpots.clear();
			parseGrindingSpotsList(m_grindingSpots);
		}

		/************************/
		/* LOM grinding section */
		/************************/

		else if (key.compare("LOM Heal") == 0)
		{
			m_Fight_LOMHeal = value;
		}
		else if (key.compare("LOM Slime A") == 0)
		{
			m_Fight_LOMSlimeA = value;
		}
		else if (key.compare("LOM Slime B") == 0)
		{
			m_Fight_LOMSlimeB = value;
		}
		else if (key.compare("LOM Slime Run") == 0)
		{
			m_Fight_LOMSlimeRun = value;
		}
		else if (key.compare("LOM turn") == 0)
		{
			m_Fight_LOMTurn = stoi(value);
		}
		else if (key.compare("LOM Slime Reset") == 0)
		{
			m_Fight_LOMReset = value;
		}

		/******************/
		/* Grasta section */
		/******************/

		else if (key.compare("Last Grasta") == 0)
		{
			m_lastGrasta = stoi(value);
		}
		else if (key.compare("Section Height") == 0)
		{
			m_SectionHeight = stoi(value);
		}
		else if (key.compare("Grasta Type") == 0)
		{
			m_grastaType = stoi(value);
		}
		else if (key.compare("Grasta to Seperate") == 0)
		{
			m_grastaNames.clear();
			parseGrastaNames(m_grastaNames);
		}

		/***************/
		/* Map section */
		/***************/
		else if (key.compare("MapsButtons") == 0)
		{
			m_Button_MapButtons.clear();
			parseNameButtons(m_Button_MapButtons);
		}


		/******************/
		/* Common section */
		/******************/
		else if (key.compare("YesButton") == 0)
		{
			parseXYinfo(value);
			m_Button_Yes = xyPosition;
		}
		else if (key.compare("LeaveButton") == 0)
		{
			parseXYinfo(value);
			m_Button_Leave = xyPosition;
		}
		else if (key.compare("XButton") == 0)
		{
			parseXYinfo(value);
			m_Button_X = xyPosition;
		}
		else if (key.compare("MenuButton") == 0)
		{
			parseXYinfo(value);
			m_Button_Menu = xyPosition;
		}
		else if (key.compare("FoodButton") == 0)
		{
			parseXYinfo(value);
			m_Button_Food = xyPosition;
		}
		else if (key.compare("AttackButton") == 0)
		{
			parseXYinfo(value);
			m_Button_Attack = xyPosition;
		}
		else if (key.compare("AFButton") == 0)
		{
			parseXYinfo(value);
			m_Button_AF = xyPosition;
		}
		else if (key.compare("MapButton") == 0)
		{
			parseXYinfo(value);
			m_Button_Map = xyPosition;
		}
		else if (key.compare("Antiquity") == 0)
		{
			parseXYinfo(value);
			m_Button_Antiquity = xyPosition;
		}
		else if (key.compare("Present") == 0)
		{
			parseXYinfo(value);
			m_Button_Present = xyPosition;
		}
		else if (key.compare("Future") == 0)
		{
			parseXYinfo(value);
			m_Button_Future = xyPosition;
		}
		else if (key.compare("EndOfTimeLoc") == 0)
		{
			parseXYinfo(value);
			m_Button_EndOfTimeLoc = xyPosition;
		}
		else if (key.compare("SpacetimeRift") == 0)
		{
			parseXYinfo(value);
			m_Button_SpacetimeRift = xyPosition;
		}
		else if (key.compare("Tree") == 0)
		{
			parseXYinfo(value);
			m_Button_Tree = xyPosition;
		}
		else if (key.compare("CharactersButtons") == 0)
		{
			m_Button_Characters.clear();
			parseButtons(m_Button_Characters);
		}
		else if (key.compare("SkillsButtons") == 0)
		{
			m_Button_Skills.clear();
			parseButtons(m_Button_Skills);
		}
	}


	if (m_IsDebug_Fighting) cout << "SkillsHorrorSet:::" << endl;
	for (auto j = 0; j < m_skillsHorrorSet.size(); j++)
	{
		if (m_IsDebug_Fighting) cout << "Turn [" << j << "]: " << m_skillsHorrorSet[j][0] << " " << m_skillsHorrorSet[j][1] << " " << m_skillsHorrorSet[j][2] << " " << m_skillsHorrorSet[j][3] << endl;
	}

	if (m_IsDebug_Fighting) cout << "SkillsMobSet:::" << endl;
	for (auto j = 0; j < m_skillsMobSet.size(); j++)
	{
		if (m_IsDebug_Fighting) cout << "Turn [" << j << "]: " << m_skillsMobSet[j][0] << " " << m_skillsMobSet[j][1] << " " << m_skillsMobSet[j][2] << " " << m_skillsMobSet[j][3] << endl;
	}

	if (m_IsDebug_Grinding) cout << "::::Grinding>>>>" << endl;
	for (auto j = 0; j < m_grindingSpots.size(); j++)
	{
		if (m_IsDebug_Grinding) cout << "--> " << m_grindingSpots[j].first << " " << m_grindingSpots[j].second << endl;
	}
	if (m_IsDebug_Grinding) cout << ">>>>Grinding::::" << endl;
}

void setup()
{
	string emulator, windowName;

	GetCurrentDirectory(MAX_PATH, m_CurrentPath);

	//Read in config file
	string key, value;
	ifstream file("config_setup.txt");
	string str;
	set<int> baitsUsedThisRun;

	vector<int> skillsRow = vector<int>(4);
	pathInfo pathStep;

	pair<int, int> xyPosition;

	auto ltrimString = [&str]()
	{
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
			return !std::isspace(ch);
			}));
	};
	auto getValueKey = [&key, &value](string& str)
	{
		int loc = (int) str.find("=");
		key = str.substr(loc + 1);
		value = str.substr(0, loc);
	};
	auto getKeyValue = [&key, &value](string& str)
	{
		int loc = (int)str.find(":");
		key = str.substr(0, loc);
		value = str.substr(loc + 1);
	};
	auto parseDynamicImage = [&ltrimString, &str, &file, &getValueKey, &getKeyValue, &key, &value]()
	{
		while (1)
		{
			std::getline(file, str);
			ltrimString();
			if (str[0] == '-' || str[0] == '\n') //skip comments and blank lines
				continue;
			if (str.compare("~END") == 0)
				break;

			imageInfo dynamicimage;

			getKeyValue(str);
			if (key.compare("id") == 0)
				dynamicimage.id = value;

			std::getline(file, str);
			ltrimString();
			getKeyValue(str);
			if (key.compare("Name") == 0)
			{
				dynamicimage.name = value;
			}

			std::getline(file, str);
			ltrimString();
			getKeyValue(str);
			if (key.compare("Width") == 0)
				dynamicimage.width = stoi(value);

			std::getline(file, str);
			ltrimString();
			getKeyValue(str);
			if (key.compare("Height") == 0)
				dynamicimage.coordy = stoi(value);

			std::getline(file, str);
			ltrimString();
			getKeyValue(str);
			if (key.compare("LocationX") == 0)
				dynamicimage.coordx = stoi(value);

			std::getline(file, str);
			ltrimString();
			getKeyValue(str);
			if (key.compare("LocationY") == 0)
				dynamicimage.coordy = stoi(value);

			std::getline(file, str);
			ltrimString();
			if (str.compare("~IMAGE") == 0)
				m_DynamicImage.push_back(dynamicimage);
		}
	};

	while (std::getline(file, str))
	{
		if (str[0] == '-' || str[0] == '\n') //skip comments and blank lines
			continue;

		getValueKey(str);

		if (key.compare("Window Name") == 0)
		{
			windowName = value;
		}
		/*****************/
		/* Mode section */
		/*****************/
		else if (key.compare("Baruoki Jump Rope") == 0)
		{
			if (stoi(value) == 1)
				runState = baruokiJumpRope;
		}
		else if (key.compare("Ratle Jump Rope") == 0)
		{
			if (stoi(value) == 1)
				runState = ratleJumpRope;
		}
		else if (key.compare("Hit Bell") == 0)
		{
			if (stoi(value) == 1)
				runState = silverHitBell_30;
			else if (stoi(value) == 2)
				runState = silverHitBell_999;
		}
		else if (key.compare("Seperate Grasta") == 0)
		{
			if (stoi(value) == 1)
				runState = seperateGrasta;
		}
		else if (key.compare("Engage Fight") == 0)
		{
			if (stoi(value) == 1)
				runState = engageFight;
		}
		else if (key.compare("Grinding") == 0)
		{
			if (stoi(value) == 1)
				runState = grindingState;
			else if (stoi(value) == 2)
				runState = grindingLOMSlimeState;
		}
		else if (key.compare("Fishing") == 0)
		{
			if (stoi(value) == 1)
				runState = fishingState;
		}
		else if (key.compare("Capture Screen") == 0)
		{
			if (stoi(value) == 1)
				runState = captureScreen;
		}
		else if (key.compare("Dynamic Image") == 0 && value.compare("x") == 0)
		{
			parseDynamicImage();
		}
	}

	cout << "RunState " << runState << endl;

	ocr = OCRTesseract::create(NULL, NULL, NULL, OEM_TESSERACT_ONLY, PSM_SINGLE_LINE);

	string innerWindowName;
	//if (emulator.compare("LD") == 0)
	//{
	emulator = "dnplayer.exe";
	innerWindowName = "TheRender";
	//}
	//else
	//{
		//emulator = "Nox.exe";
		//innerWindowName = "ScreenBoardClassWindow";
	//}

	pair<string*, string*> enumInput = make_pair(&emulator, &windowName);
	EnumWindows(EnumWindowsProc, LPARAM(&enumInput));
	EnumChildWindows(window, EnumChildWindowsProc, LPARAM(&innerWindowName));

	RECT rect;
	GetWindowRect(window, &rect);
	height = rect.bottom - rect.top;
	width = rect.right - rect.left;
	xCenter = width / 2;
	yCenter = height / 2;
	heightPct = height / 981.0;
	widthPct = width / 1745.0;

	fishIcon = imread("images\\fish.png", IMREAD_UNCHANGED);
	exclamationIcon = imread("images\\exclamation.png", IMREAD_UNCHANGED);
	exclRiftIcon = imread("images\\exclamationRift.png", IMREAD_UNCHANGED);
	exclSepcterIcon = imread("images\\exclamationSepcter.png", IMREAD_UNCHANGED);
	exclGrastaIcon = imread("images\\exclamationGrasta.png", IMREAD_UNCHANGED);
	exclChamberIcon = imread("images\\exclamationChamber.png", IMREAD_UNCHANGED);
	exclChamberPlasmaIcon = imread("images\\exclamationChamberPlasma.png", IMREAD_UNCHANGED);
	exclKMSIcon = imread("images\\exclamationKMS.png", IMREAD_UNCHANGED);
	swampFishIcon = imread("images\\swampFish.png", IMREAD_UNCHANGED);
	doorDefaultIcon = imread("images\\doorDefault.png", IMREAD_UNCHANGED);
	doorKunlunIcon = imread("images\\doorKunlun.png", IMREAD_UNCHANGED);
	doorLOMIcon = imread("images\\doorLOM.png", IMREAD_UNCHANGED);
	stairDefaultIcon = imread("images\\stairDefault.png", IMREAD_UNCHANGED);
	stairDownIcon = imread("images\\stairDown.png", IMREAD_UNCHANGED);
	stairLOMIcon = imread("images\\stairLOM.png", IMREAD_UNCHANGED);

	afBarEmptyPic = imread("images\\AEBarEmpty.png", IMREAD_UNCHANGED);
	afBarFullPic = imread("images\\AEBarFull.png", IMREAD_UNCHANGED);
	hitBellPic = imread("images\\Bell.png", IMREAD_UNCHANGED);
	jmpRopePic1 = imread("images\\jmpRopePic1.png", IMREAD_UNCHANGED);
	jmpRopePic2 = imread("images\\jmpRopePic2.png", IMREAD_UNCHANGED);
	jmpRopePic3 = imread("images\\jmpRopePic3.png", IMREAD_UNCHANGED);
	jmpRopePic4 = imread("images\\jmpRopePic4.png", IMREAD_UNCHANGED);

	for (auto i = 0; i < m_DynamicImage.size(); i++)
	{
		m_DynamicImage[i].name = string("images\\") + m_DynamicImage[i].name;
		m_DynamicImage[i].image = imread(m_DynamicImage[i].name, IMREAD_UNCHANGED);
	}

	m_MonsterVec_Baruoki.push_back(imread("images\\monster_baruoki.png", IMREAD_UNCHANGED));
	m_MonsterVec_Acteul.push_back(imread("images\\monster_acteul.png", IMREAD_UNCHANGED));
	m_MonsterVec_Vasu.push_back(imread("images\\monster_vasu.png", IMREAD_UNCHANGED));
	m_MonsterVec_Serena.push_back(imread("images\\monster_serena.png", IMREAD_UNCHANGED));
	m_MonsterVec_Rucyana.push_back(imread("images\\monster_rucyana.png", IMREAD_UNCHANGED));
	m_MonsterVec_Elzion.push_back(imread("images\\monster_elzion.png", IMREAD_UNCHANGED));
	m_MonsterVec_LastIsland.push_back(imread("images\\monster_lastisland.png", IMREAD_UNCHANGED));
	m_MonsterVec_DimensionRift.push_back(imread("images\\monster_dimensionrift1.png", IMREAD_UNCHANGED));
	m_MonsterVec_DimensionRift.push_back(imread("images\\monster_dimensionrift2.png", IMREAD_UNCHANGED));
	m_MonsterVec_DimensionRift.push_back(imread("images\\monster_dimensionrift3.png", IMREAD_UNCHANGED));
	m_MonsterVec_LOMPSlime.push_back(imread("images\\monster_lompslime1.png", IMREAD_UNCHANGED));
	m_MonsterVec_LOMPSlime.push_back(imread("images\\monster_lompslime2.png", IMREAD_UNCHANGED));

	if (heightPct != 1.0 || widthPct != 1.0)
	{
		resize(fishIcon, fishIcon, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(exclamationIcon, exclamationIcon, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(exclRiftIcon, exclRiftIcon, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(exclSepcterIcon, exclSepcterIcon, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(exclGrastaIcon, exclGrastaIcon, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(exclChamberIcon, exclChamberIcon, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(exclChamberPlasmaIcon, exclChamberPlasmaIcon, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(exclKMSIcon, exclKMSIcon, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(swampFishIcon, swampFishIcon, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(doorDefaultIcon, doorDefaultIcon, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(doorKunlunIcon, doorKunlunIcon, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(doorLOMIcon, doorLOMIcon, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(stairDefaultIcon, stairDefaultIcon, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(stairDownIcon, stairDownIcon, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(stairLOMIcon, stairLOMIcon, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);

		resize(afBarEmptyPic, afBarEmptyPic, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(afBarFullPic, afBarFullPic, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(hitBellPic, hitBellPic, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(jmpRopePic1, jmpRopePic1, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(jmpRopePic2, jmpRopePic2, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(jmpRopePic3, jmpRopePic3, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(jmpRopePic4, jmpRopePic4, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);

		resize(m_MonsterVec_Baruoki[0], m_MonsterVec_Baruoki[0], Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(m_MonsterVec_Acteul[0], m_MonsterVec_Acteul[0], Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(m_MonsterVec_Vasu[0], m_MonsterVec_Vasu[0], Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(m_MonsterVec_Serena[0], m_MonsterVec_Serena[0], Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(m_MonsterVec_Rucyana[0], m_MonsterVec_Rucyana[0], Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(m_MonsterVec_Elzion[0], m_MonsterVec_Elzion[0], Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(m_MonsterVec_LastIsland[0], m_MonsterVec_LastIsland[0], Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(m_MonsterVec_DimensionRift[0], m_MonsterVec_DimensionRift[0], Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(m_MonsterVec_DimensionRift[1], m_MonsterVec_DimensionRift[1], Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(m_MonsterVec_DimensionRift[2], m_MonsterVec_DimensionRift[2], Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);

		resize(m_MonsterVec_LOMPSlime[0], m_MonsterVec_LOMPSlime[0], Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
		resize(m_MonsterVec_LOMPSlime[1], m_MonsterVec_LOMPSlime[1], Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);

		for (auto i = 0; i < m_DynamicImage.size(); i++)
			resize(m_DynamicImage[i].image, m_DynamicImage[i].image, Size(), widthPct, heightPct, heightPct >= 1.0 ? INTER_CUBIC : INTER_AREA);
	}

	rng = std::mt19937(dev());
	lClickPixelRand = std::uniform_int_distribution<std::mt19937::result_type>(0, (unsigned int) round(widthPct * 7));
	shortSleepRand = std::uniform_int_distribution<std::mt19937::result_type>(0, 10);
	sleepRand = std::uniform_int_distribution<std::mt19937::result_type>(0, 50);
	slideSleepRand = std::uniform_int_distribution<std::mt19937::result_type>(0, 1);
	slideLClick = std::uniform_int_distribution<std::mt19937::result_type>(0, (unsigned int) round(widthPct * 30));
	slideRand = std::uniform_int_distribution<std::mt19937::result_type>(0, 2);
	boolRand = std::uniform_int_distribution<std::mt19937::result_type>(0, 1);
	slideDistanceRand = std::uniform_int_distribution<std::mt19937::result_type>(0, 100);
	longSleepRand = std::uniform_int_distribution<std::mt19937::result_type>(0, 1000);

	hdc = GetWindowDC(window);
	hDest = CreateCompatibleDC(hdc);

	void* ptrBitmapPixels;

	BITMAPINFO bi;
	ZeroMemory(&bi, sizeof(BITMAPINFO));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = width;
	bi.bmiHeader.biHeight = -height;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	hbDesktop = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &ptrBitmapPixels, NULL, 0);

	SelectObject(hDest, hbDesktop);

	bitbltPic = Mat(height, width, CV_8UC4, ptrBitmapPixels, 0);
}

void loadFishingConfig()
{
	m_fishingSpots.clear();
	m_baitList.clear();

	//Read in config file
	string key, value;
	ifstream file("config_fishing.txt");
	string str;
	set<int> baitsUsedThisRun;

	pathInfo pathStep;

	//Set up baits
	for (int i = Fishing_Dango; i < Premium_Crab_Cake + 1; ++i)
	{
		m_baitList.push_back(make_pair(false, 0));
	}

	auto ltrimString = [&str]()
	{
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
			return !std::isspace(ch);
			}));
	};
	auto getValueKey = [&key, &value](string& str)
	{
		int loc = (int)str.find("=");
		key = str.substr(loc + 1);
		value = str.substr(0, loc);
	};
	auto parseBaitForArea = [&ltrimString, &str, &file, &getValueKey, &key, &value, &baitsUsedThisRun](fishingSpot& area)
	{
		while (1)
		{
			std::getline(file, str);
			ltrimString();
			if (str[0] == '-' || str[0] == '\n') //skip comments and blank lines
				continue;
			if (str.compare("~END") == 0)
				break;

			getValueKey(str);
			if (stoi(value) > 0)
			{
				if (key.compare("Fishing Dango") == 0)
				{
					area.baitsToUse.push_back(Fishing_Dango);
					baitsUsedThisRun.insert(Fishing_Dango);
				}
				else if (key.compare("Worm") == 0)
				{
					area.baitsToUse.push_back(Worm);
					baitsUsedThisRun.insert(Worm);
				}
				else if (key.compare("Unexpected Worm") == 0)
				{
					area.baitsToUse.push_back(Unexpected_Worm);
					baitsUsedThisRun.insert(Unexpected_Worm);
				}
				else if (key.compare("Shopaholic Clam") == 0)
				{
					area.baitsToUse.push_back(Shopaholic_Clam);
					baitsUsedThisRun.insert(Shopaholic_Clam);
				}
				else if (key.compare("Spree Snail") == 0)
				{
					area.baitsToUse.push_back(Spree_Snail);
					baitsUsedThisRun.insert(Spree_Snail);
				}
				else if (key.compare("Dressed Crab") == 0)
				{
					area.baitsToUse.push_back(Dressed_Crab);
					baitsUsedThisRun.insert(Dressed_Crab);
				}
				else if (key.compare("Tear Crab") == 0)
				{
					area.baitsToUse.push_back(Tear_Crab);
					baitsUsedThisRun.insert(Tear_Crab);
				}
				else if (key.compare("Foamy Worm") == 0)
				{
					area.baitsToUse.push_back(Foamy_Worm);
					baitsUsedThisRun.insert(Foamy_Worm);
				}
				else if (key.compare("Bubbly Worm") == 0)
				{
					area.baitsToUse.push_back(Bubbly_Worm);
					baitsUsedThisRun.insert(Bubbly_Worm);
				}
				else if (key.compare("Snitch Sardines") == 0)
				{
					area.baitsToUse.push_back(Snitch_Sardines);
					baitsUsedThisRun.insert(Snitch_Sardines);
				}
				else if (key.compare("Blabber Sardines") == 0)
				{
					area.baitsToUse.push_back(Blabber_Sardines);
					baitsUsedThisRun.insert(Blabber_Sardines);
				}
				else if (key.compare("Crab Cake") == 0)
				{
					area.baitsToUse.push_back(Crab_Cake);
					baitsUsedThisRun.insert(Crab_Cake);
				}
				else if (key.compare("Premium Crab Cake") == 0)
				{
					area.baitsToUse.push_back(Premium_Crab_Cake);
					baitsUsedThisRun.insert(Premium_Crab_Cake);
				}
			}
		}
	};

	vector<pair<int, void(*)(void)>> fishingSpotsInput;
	bool inArea = false;
	while (std::getline(file, str))
	{
		if (str[0] == '-' || str[0] == '\n') //skip comments and blank lines
			continue;

		getValueKey(str);

		if (key.compare("Fishing Dango Max") == 0)
			m_baitList[Fishing_Dango].second = stoi(value);
		else if (key.compare("Worm Max") == 0)
			m_baitList[Worm].second = stoi(value);
		else if (key.compare("Unexpected Worm Max") == 0)
			m_baitList[Unexpected_Worm].second = stoi(value);
		else if (key.compare("Shopaholic Clam Max") == 0)
			m_baitList[Shopaholic_Clam].second = stoi(value);
		else if (key.compare("Spree Snail Max") == 0)
			m_baitList[Spree_Snail].second = stoi(value);
		else if (key.compare("Dressed Crab Max") == 0)
			m_baitList[Dressed_Crab].second = stoi(value);
		else if (key.compare("Tear Crab Max") == 0)
			m_baitList[Tear_Crab].second = stoi(value);
		else if (key.compare("Foamy Worm Max") == 0)
			m_baitList[Foamy_Worm].second = stoi(value);
		else if (key.compare("Bubbly Worm Max") == 0)
			m_baitList[Bubbly_Worm].second = stoi(value);
		else if (key.compare("Snitch Sardines Max") == 0)
			m_baitList[Snitch_Sardines].second = stoi(value);
		else if (key.compare("Blabber Sardines Max") == 0)
			m_baitList[Blabber_Sardines].second = stoi(value);
		else if (key.compare("Crab Cake Max") == 0)
			m_baitList[Crab_Cake].second = stoi(value);
		else if (key.compare("Premium Crab Cake Max") == 0)
			m_baitList[Premium_Crab_Cake].second = stoi(value);


		else if (key.compare("Ancient Battlefield") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_ancientBattlefield, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Baruoki") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_baruoki, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Acteul") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_acteul, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Charol Plains") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_charolPlains, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Elzion Airport") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_elzionAirport, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Dimension Rift") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_dimensionRift, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Dragon Palace - Inner Wall") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_innerWall, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Dragon Palace Past - Inner Wall") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_innerWallPast, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Dragon Palace - Outer Wall Bamboo") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_outerWallBamboo, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Dragon Palace Past - Outer Wall Bamboo") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_outerWallBambooPast, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Dragon Palace - Outer Wall Pine") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_outerWallPine, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Dragon Palace Past - Outer Wall Pine") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_outerWallPinePast, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Dragon Palace - Outer Wall Plum") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_outerWallPlum, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Dragon Palace Past - Outer Wall Plum") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_outerWallPlumPast, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Karek Swampland") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_karekSwampland, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Kira Beach") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_kiraBeach, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Lake Tillian") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_lakeTillian, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Last Island") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_lastIsland, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Man-Eating Swamp") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_manEatingSwamp, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Moonlight Forest") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_moonlightForest, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Naaru Uplands") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_naaruUplands, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Nilva") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_nilva, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Rinde") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_rinde, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Rucyana Sands") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_rucyanaSands, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Serena Coast") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_serenaCoast, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Snake Neck Igoma") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_snakeNeckIgoma, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Vasu Mountains") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_vasuMountain, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Zol Plains") == 0 && stoi(value) > 0)
		{
			m_fishingSpots.push_back(fishingSpot(&fishing_zolPlains, stoi(value), key));
			parseBaitForArea(m_fishingSpots.back());
		}
		else if (key.compare("Fight Horror") == 0)
		{
			if (stoi(value) == 1)
			{
				isFightHorror = true;
			}
		}
	}

	if (m_IsDebug_Fishing) cout << "::::Fishing>>>>" << endl;
	for (auto j = 0; j < m_fishingSpots.size(); j++)
	{
		if (m_IsDebug_Fishing) cout << "--> " << m_fishingSpots[j].locationName << endl;
	}
	if (m_IsDebug_Fishing) cout << ">>>>Fishing::::" << endl;

	std::sort(m_fishingSpots.begin(), m_fishingSpots.end(), [](fishingSpot& lhs, fishingSpot& rhs) { return lhs.orderNumber < rhs.orderNumber; });

	//Check to see if any spots actually use the bait we've selected to buy. If not, don't buy any for this run
	for (int i = 0; i < m_baitList.size(); ++i)
	{
		if (baitsUsedThisRun.find(i) == baitsUsedThisRun.end())
			m_baitList[i].second = 0;
	}
}

void loadPathConfig()
{
	//Read in config file
	m_LocationList.clear();

	pathInfo pathStep;
	ifstream file("config_path.txt");
	string str;
	string key, value;

	auto ltrimString = [&str]()
	{
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
			return !std::isspace(ch);
			}));
	};
	auto getKeyValue = [&key, &value](string& str)
	{
		int loc = (int)str.find(":");
		key = str.substr(0, loc);
		value = str.substr(loc + 1);
	};
	auto parsePathStep = [&pathStep](string& str)
	{
		int loc = (int)str.find("=");
		pathStep.type = str.substr(0, loc);
		string val = str.substr(loc + 1);

		size_t pos = 0;
		pos = val.find(",");
		if (pos != std::string::npos)
		{
			pathStep.value1 = val.substr(0, pos);
			pathStep.value2 = val.substr(pos + 1);
		}
		else
		{
			pathStep.value1 = val;
			pathStep.value2.clear();
		}
	};

	auto parsePathList = [&ltrimString, &str, &file, &pathStep, &parsePathStep](locationInfo& location)
	{
		while (1)
		{
			std::getline(file, str);
			ltrimString();
			if (str[0] == '-' || str[0] == '\n') //skip comments and blank lines
				continue;
			if (str.compare("~END") == 0)
				break;

			parsePathStep(str);
			location.pathStepsList.push_back(pathStep);
		}
	};

	while (std::getline(file, str))
	{
		if (str[0] == '-' || str[0] == '\n') //skip comments and blank lines
			continue;

		getKeyValue(str);

		if (key.compare("Location") == 0)
		{
			locationInfo locInfo;
			locInfo.locationName = value;
			m_LocationList.push_back(locInfo);
			parsePathList(m_LocationList.back());
		}
	}
}

void convertPic(string inName, string outName)
{
	Mat tmp = imread(inName, IMREAD_UNCHANGED);
	Mat newPic;
	cvtColor(tmp, newPic, COLOR_BGR2BGRA, 4); //Conversion to 4 channel
	imwrite(outName, newPic);
	exit(0);
}

void stateCaptureScreen()
{
	captureScreenNow();
}

void stateSilverHitBell(botState silverHitBellstate)
{
	Mat bellPicCrop;
	int MSD1;
	int lowMSD = (int)99999999999;

	while (1)
	{
		int i = 0;

		SendMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(xCenter, yCenter));
		sleepR(1);
		SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(xCenter, yCenter));
		Sleep(3000);

		SendMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(xCenter, yCenter));
		sleepR(1);
		SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(xCenter, yCenter));

		Sleep(1200);

		SendMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(xCenter, yCenter));
		sleepR(1);
		SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(xCenter, yCenter));
		i++;

		Sleep(1350);

		SendMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(xCenter, yCenter));
		sleepR(1);
		SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(xCenter, yCenter));
		i++;

		for (; i < 4;) {
			Sleep(1350);

			SendMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(xCenter, yCenter));
			sleepR(1);
			SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(xCenter, yCenter));
			i++;
		}

		int j = 750;
		Sleep(j);
		while (1)
		{
			if (silverHitBellstate == silverHitBell_30 && i > 30)
				break;
			else if (silverHitBellstate == silverHitBell_999 && i > 999)
				return;

			if (j > 2000)
				break;

			Sleep(1);
			bitBltWholeScreen();

			copyPartialPic(bellPicCrop, 160, 80, 1095, 323);
			MSD1 = (int)cv::norm(hitBellPic, bellPicCrop);
			MSD1 = MSD1 * MSD1 / (int)hitBellPic.total();

			if (MSD1 < msdThreshold)
			{
				int tosleep = 1;
				Sleep(tosleep);
				SendMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(xCenter, yCenter));
				sleepR(1);
				SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(xCenter, yCenter));
				j = 800;
				Sleep(j);
				i++;
			}
			else
			{
				j = j + 25;
				Sleep(25);
			}
		}

		leftClick(xCenter, yCenter); // miss the bell
		Sleep(10000);

		leftClick(xCenter, yCenter);
		Sleep(4000);
		leftClick(xCenter, yCenter);
		leftClick(xCenter, yCenter);
		Sleep(2000);

		// Click Icon
		leftClick(613, 286, 5000);
		// Click Msg
		leftClick(xCenter, yCenter);
		// Click Yes
		leftClick(m_Button_Yes, 5000);
		// After Click Yes 1
		leftClick(xCenter, yCenter, 5000);
		// After Click Yes 2
		leftClick(xCenter, yCenter, 5000);
	}

}

void stateJumpRopeRatle()
{
	Mat jmpRopePicCrop;
	int MSD1;
	int lowMSD = (int) 99999999999;

	bool check = false;

	SendMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(xCenter, yCenter));
	sleepR(1);
	SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(xCenter, yCenter));

	Sleep(1000);

	int q = 0;
	while (1)
	{
		Sleep(1);
		bitBltWholeScreen();

		if (check) {
			copyPartialPic(jmpRopePicCrop, 21, 109, 710, 575);
			MSD1 = (int) cv::norm(jmpRopePic2, jmpRopePicCrop);
			MSD1 = MSD1 * MSD1 / (int) jmpRopePic2.total();

			if (MSD1 > 1000)
			{
				check = false;

				SendMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(xCenter, yCenter));
				sleepR(1);
				SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(xCenter, yCenter));
				if (q++ == 10001)
					break;
			}
		}
		else
		{
			copyPartialPic(jmpRopePicCrop, 25, 79, 745, 305);
			MSD1 = (int) cv::norm(jmpRopePic1, jmpRopePicCrop);
			MSD1 = MSD1 * MSD1 / (int) jmpRopePic1.total();

			if (MSD1 > 1000)
			{
				check = true;
				Sleep(200);
			}
		}
	}
}

void stateJumpRopeBaruoki()
{
	Mat jmpRopePicCrop;
	int MSD1;
	int lowMSD = (int) 99999999999;

	bool check = true;

	SendMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(xCenter, yCenter));
	sleepR(1);
	SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(xCenter, yCenter));

	Sleep(1000);

	int q = 0;
	while (1)
	{
		Sleep(1);
		bitBltWholeScreen();

		if (check) {
			copyPartialPic(jmpRopePicCrop, 12, 108, 761, 585);
			MSD1 = (int) cv::norm(jmpRopePic4, jmpRopePicCrop);
			MSD1 = MSD1 * MSD1 / (int) jmpRopePic4.total();

			if (MSD1 > 1000)
			{
				check = false;

				SendMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(xCenter, yCenter));
				sleepR(1);
				SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(xCenter, yCenter));
				if (q++ == 10001)
					break;
			}
		}
		else
		{
			copyPartialPic(jmpRopePicCrop, 44, 20, 958, 324);
			MSD1 = (int) cv::norm(jmpRopePic3, jmpRopePicCrop);
			MSD1 = MSD1 * MSD1 / (int) jmpRopePic3.total();

			if (MSD1 > 1000)
			{
				check = true;
				Sleep(200);
			}
		}
	}
}

void stateSeparateGrasta()
{
	auto getGrastaText = [](int columns, int rows, int x, int y)
	{
		bitBltWholeScreen();
		cv::Mat partialPic;
		copyPartialPic(partialPic, columns, rows, x, y);
		return getText(partialPic);
	};

	auto getGrastaNumber = [](int columns, int rows, int x, int y)
	{
		bitBltWholeScreen();
		cv::Mat partialPic;
		copyPartialPic(partialPic, columns, rows, x, y);
		return getNumber(partialPic);
	};

	longSleepR(1000);

	pair<int, int> pnt = findExclamationIcon(exclGrasta);
	leftClick(pnt.first, pnt.second, 2000, false);

	leftClick(880, 530);

	if (m_grastaType == grasta_Life)
		leftClick(535, 290);
	else if (m_grastaType == grasta_Support)
		leftClick(535, 460);
	else if (m_grastaType == grasta_Special)
		leftClick(535, 630);

	//Buy baits if needed
	const static vector<pair<int, int>> ItemLocs = { {1220, 160}, {1215, 325}, {1215, 465}, {1215, 625}, {1215, 800}, {1225, 925} };

	if (m_IsDebug_Grasta) cout << "To separate grasta from [" << m_lastGrasta << "]" << endl;

	int i = 0;
	//Only 5  slots are fully on screen at a time, with the 6th partially visible. For every one after 6, scroll down until new one occupies bottom slot and then attempt to check

	if (m_IsDebug_Grasta) cout << "Last Grasta " << m_lastGrasta << endl;

	int k=0;

	while (1)
	{
		if (k < 5)
		{
			leftClick(ItemLocs[k].first, ItemLocs[k].second, 200);
		}
		else
		{
			if (k < m_lastGrasta)
			{
				if (m_IsDebug_Grasta) cout << "Goto [" << m_lastGrasta << "]..." << endl;
				drag(DOWN, m_SectionHeight * (m_lastGrasta - 5) + 60, 1225, 900);
				k = m_lastGrasta;
			}
			else
			{
				if (k == 5)
					drag(DOWN, 60, 1225, 900);
				else 
					drag(DOWN, 135, 1225, 900);
			}
			leftClick(ItemLocs[5].first, ItemLocs[5].second, 200);
		}

		string txt;
		//If the next one we've done is the same as the last, we've reached the end of the list
		if (m_grastaType == grasta_Attack)
			txt = getGrastaText(577, 51, 100, 102);
		else if (m_grastaType == grasta_Life)
			txt = getGrastaText(577, 51, 100, 102);

		string attr_1 = getGrastaText(110, 30, 390, 170);
		int value_1 = getGrastaNumber(110, 30, 565, 170);
		string attr_2 = getGrastaText(110, 30, 390, 225);
		int value_2 = getGrastaNumber(110, 30, 565, 225);

		//found grasta to separate
		if (m_IsDebug_Grasta) cout << "Grasta [" << k << "], " << txt << ", " << attr_1 << ", " << value_1 << ", " << attr_2 << ", " << value_2 << endl;

		for (auto j = 0; j < m_grastaNames.size(); j++)
		{
			if (txt.find(m_grastaNames[j]) != string::npos)
			{
				m_lastGrasta = k;
				if (m_IsDebug_Grasta) cout << "To separate [" << m_lastGrasta << "]..." << endl;
				leftClick(350, 870, 500);
				leftClick(1080, 600, 500);
				leftClick(880, 735, 500);
				longSleepR(1000);
				k = 0;

				break;
			}
		}
		k++;
	}

	leftClick(m_Button_X);
}

int grindingRun()
{
	struct tm timeinfo;
	char buf[80];
	char buflastfight[80];
	time_t rawtime, lastFight;

	rawtime = time(NULL);
	localtime_s(&timeinfo, &rawtime);
	strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);

	if (m_IsDebug_Grinding) cout << "Start grinding " << currentLocation << "[" << m_Fight_EndlessGrinding  << "] " << buf << endl;

	lastFight = rawtime;
	strftime(buflastfight, sizeof(buflastfight), "%Y-%m-%d_%H-%M-%S", &timeinfo);

	while (currentGrindingCounter < m_Fight_GrindingCount || m_Fight_EndlessGrinding)
	{
		while (!inBattle())
		{
			rawtime = time(NULL);
			localtime_s(&timeinfo, &rawtime);
			strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);

			auto timegap = difftime(rawtime, lastFight);

			if (timegap > m_Fight_Timeout) // return if timeout
			{
				if (m_IsDebug_Grinding) cout << "Leaving " << currentLocation << " - Idling "<< buflastfight << " : " << buf << endl;

				if (m_IsPrint) captureScreenNow("Idling");

				return status_Timeout;
			}

			if (m_Fight_GrindingDirection == LR)
			{
				Walk(LEFT, m_Fight_GrindingStep, 200);
				Walk(RIGHT, m_Fight_GrindingStep, 200);
			}
			else
			{
				Walk(m_Fight_GrindingDirection, m_Fight_GrindingStep, 200);
			}
		}

		engageMobFightNow();

		lastFight = time(NULL);
		localtime_s(&timeinfo, &lastFight);
		strftime(buflastfight, sizeof(buflastfight), "%Y-%m-%d_%H-%M-%S", &timeinfo);
	}

	rawtime = time(NULL);
	localtime_s(&timeinfo, &rawtime);
	strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);

	if (m_IsDebug_Grinding) cout << "Leaving " << currentLocation << " (" << currentGrindingCounter << ") - Counter Number reached " << buf << endl;
	return status_NoError;
}

void stateGrindingLOMSlime()
{
	while (1)
	{
		loadPathConfig();
		loadSettingConfig();

		m_grindingSpots.clear(); // re config the grinding spots

		if (!m_Fight_LOMHeal.empty())
			m_grindingSpots.push_back(make_pair(m_Fight_LOMHeal, 1));

		if (!m_Fight_LOMSlimeA.empty())
			m_grindingSpots.push_back(make_pair(m_Fight_LOMSlimeA, 1));

		if (!m_Fight_LOMHeal.empty())
			m_grindingSpots.push_back(make_pair(m_Fight_LOMHeal, 1));

		if (!m_Fight_LOMSlimeB.empty())
			m_grindingSpots.push_back(make_pair(m_Fight_LOMSlimeB, 1));

		if (!m_Fight_LOMSlimeRun.empty())
			m_grindingSpots.push_back(make_pair(m_Fight_LOMSlimeRun, m_Fight_LOMTurn));

		int resValue = status_NoError;
		int resetindex = -1;
		if (!m_Fight_LOMReset.empty())
		{
			for (auto j = 0; j < m_LocationList.size(); j++)
			{
				// if this location is the right one to grind
				if (m_LocationList[j].locationName.compare(m_Fight_LOMReset) == 0)
				{
					resetindex = j;
					break;
				}
			}
		}

		if (m_IsDebug_LOM) cout << "::::Grinding LOM Slime>>>>" << endl;
		for (auto i = 0; i < m_grindingSpots.size(); i++)
		{
			if (m_IsDebug_LOM) cout << "--> " << m_grindingSpots[i].first << " " << m_grindingSpots[i].second << endl;
		}
		if (m_IsDebug_LOM) cout << ">>>>Grinding LOM Slime::::" << endl;

		for (auto i = 0; i < m_grindingSpots.size(); i++)
		{
			currentLocation = m_grindingSpots[i].first;
			for (auto j = 0; j < m_LocationList.size(); j++)
			{
				// if this location is the right one to grind
				if (m_LocationList[j].locationName.compare(m_grindingSpots[i].first) == 0)
				{

					for (auto k = 0; k < m_grindingSpots[i].second; k++)
					{
						resValue = goToTargetLocation(m_LocationList[j].pathStepsList); //load X 1280 Y 720 coordination 

						if (resValue == status_Timeout)
						{
							if (m_IsDebug_LOM) cout << "Grinding LOM Reset " << currentLocation << endl;
							goToTargetLocation(m_LocationList[resetindex].pathStepsList); // reset
						}
					}
					break;
				}
			}
		}

		//Wait to go back for refresh
		Sleep(3000);
	}
}

void stateGrinding()
{
	while (1)
	{
		loadPathConfig();
		loadSettingConfig();

		for (auto i = 0; i < m_grindingSpots.size(); i++)
		{
			currentLocation = m_grindingSpots[i].first;

			for (auto j = 0; j < m_LocationList.size(); j++)
			{
				// if this location is the right one to grind
				if (m_LocationList[j].locationName.compare(currentLocation) == 0)
				{
					for (auto k = 0; k < m_grindingSpots[i].second; k++)
					{
						currentGrindingCounter = 0;
						if (m_Fight_GoToSpacetimeRift) // no need to go to Spacetime Rift
							goToSpacetimeRift();
						goToTargetLocation(m_LocationList[j].pathStepsList); //load X 1280 Y 720 coordination 
						grindingRun();
					}
				}
			}
		}

		//Wait to go back for refresh
		Sleep(3000);
	}
}

void stateFishing()
{
	while (1)
	{
		loadFishingConfig();
		loadPathConfig();
		loadSettingConfig();

		goToFishVendor();
		for (int i = 0; i < m_fishingSpots.size(); ++i)
		{
			currentBaitsToUse = &(m_fishingSpots[i].baitsToUse);
			currentLocation = m_fishingSpots[i].locationName;
			(*m_fishingSpots[i].fishFunction)();
		}
	}
}

int main()
{
	setup();
	loadSettingConfig();

	cout << "runState is " << runState << endl;

	switch (runState) {
	case baruokiJumpRope:
		stateJumpRopeBaruoki();
		break;
	case ratleJumpRope:
		stateJumpRopeRatle();
		break;
	case silverHitBell_30:
	case silverHitBell_999:
		stateSilverHitBell(runState);
		break;
	case seperateGrasta:
		stateSeparateGrasta();
		break;
	case engageFight:
		engageHorrorFightNow();
		break;
	case grindingState:
		stateGrinding();
		break;
	case grindingLOMSlimeState:
		stateGrindingLOMSlime();
		break;
	case fishingState:
		stateFishing();
		break;
	case captureScreen:
	default:
		stateCaptureScreen();
		break;
	}
}
