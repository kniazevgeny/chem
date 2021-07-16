// Fill out your copyright notice in the Description page of Project Settings.
#include "MyActor2.h"
#include "ParseMolecule.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Engine.h"
#include <cstring>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <stdexcept>

std::string prev = "";
int prevRow = 2;
std::vector<AActor *> ActorsList;
std::vector<std::vector<int>> AdjacencyList;

std::vector<std::vector<int>> graph;
std::vector<std::string> decodeInfo;

// index of molecule/graph in molecules[]
int nOfGraph = 0;

FVector circleFocus = {0.f, 0.f, 20.f};
int circleRadius = 175;
float circleAngle = 0;

bool isStarted = false;

std::vector<FString> elementClassPath = {
    "",
    "/Game/Blueprints/Hydrogen.Hydrogen_C",
    "/Game/Blueprints/Helium.Helium_C",
    "/Game/Blueprints/Lithium.Lithium_C",
    "/Game/Blueprints/Beryllium.Beryllium_C",
    "/Game/Blueprints/Boron.Boron_C",
    "/Game/Blueprints/Carbon.Carbon_C",
    "/Game/Blueprints/Nitrogen.Nitrogen_C",
    "/Game/Blueprints/Oxygen.Oxygen_C",
    "/Game/Blueprints/Fluorine.Fluorine_C",
    "/Game/Blueprints/Neon.Neon_C",
    "/Game/Blueprints/Sodium.Sodium_C",
    "/Game/Blueprints/Magnesium.Magnesium_C",
    "/Game/Blueprints/Aluminium.Aluminium_C",
    "/Game/Blueprints/Silicon.Silicon_C",
    "/Game/Blueprints/Phosphorus.Phosphorus_C",
    "/Game/Blueprints/Sulfur.Sulfur_C",
    "/Game/Blueprints/Chlorine.Chlorine_C",
    "/Game/Blueprints/Argon.Argon_C"

};

std::vector<FString> elementMaterialPath = {
    "",
    "Material'/Game/Collections/Hydrogen.Hydrogen'",
    "Material'/Game/Collections/Helium.Helium'",
    "Material'/Game/Collections/Lithium.Lithium'",
    "Material'/Game/Collections/Beryllium.Beryllium'",
    "Material'/Game/Blueprints/Boron.Boron'",
    "Material'/Game/Blueprints/Carbon.Carbon'",
    "Material'/Game/Blueprints/Nitrogen.Nitrogen'",
    "Material'/Game/Blueprints/Oxygen.Oxygen'",
    "Material'/Game/Blueprints/Fluorine.Fluorine'",
    "Material'/Game/Blueprints/Neon.Neon'",
    "Material'/Game/Blueprints/Sodium.Sodium'",
    "Material'/Game/Blueprints/Magnesium.Magnesium'",
    "Material'/Game/Blueprints/Aluminium.Aluminium'",
    "Material'/Game/Blueprints/Silicon.Silicon'",
    "Material'/Game/Blueprints/Phosphorus.Phosphorus'",
    "Material'/Game/Blueprints/Sulfur.Sulfur'",
    "Material'/Game/Blueprints/Chlorine.Chlorine'",
    "Material'/Game/Blueprints/Argon.Argon'"

};

std::vector<std::string> atomName = {
    "",
    "H",
    "He",
    "Li",
    "Be",
    "B",
    "C",
    "N",
    "O",
    "F",
    "Ne",
    "Na",
    "Mg",
    "Al",
    "Si",
    "P",
    "S",
    "Cl",
    "Ar",
    "K",
    "Ca",
    "Sc",
    "Ti",
    "V",
    "Cr",
    "Mn",
    "Fe",
    "Co",
    "Ni",
    "Cu",
    "Zn",
};

AMyActor2::AMyActor2()
{
  //int log = std::stoi(names[0]);
  //FString HappyString(names[0].c_str());
  //GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("a= %s"), HappyString));
  // prev = TCHAR_TO_UTF8(*compound);
}

AActor *AMyActor2::SpawnObject(FVector Loc, FRotator Rot, int elementIndex)
{
  //BPEvent_TestCall(elementIndex);
  UE_LOG(LogTemp, Warning, TEXT("Element index is %i"), elementIndex);
  FActorSpawnParameters SpawnParams;
  // TODO: create an array with classes paths,
  // and then choose from them a name by an periodic el number

  // UObject *SpawnActor = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, *elementClassPath[elementIndex]));
  // UBlueprint *GeneratedBP = Cast<UBlueprint>(SpawnActor);
  // if (!SpawnActor)
  // {
  //   GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("CANT FIND OBJECT TO SPAWN")));
  //   throw std::invalid_argument("Can't find object to spawn");
  // }

  UClass *SpawnClass = StaticLoadClass(UObject::StaticClass(), NULL, *elementClassPath[elementIndex]);
  if (SpawnClass == NULL)
  {
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("CLASS == NULL")));
    throw std::invalid_argument("Class == null");
  }
  AActor *SpawnedActorRef = GetWorld()->SpawnActor<AActor>(SpawnClass, {Loc.X, Loc.Y, Loc.Z}, Rot, SpawnParams);
  ActorsList.push_back(SpawnedActorRef);
  // if (ActorsList.size() == 3)
  //   circleFocus = SpawnedActorRef->GetActorLocation();
  // Add object reference return value
  return SpawnedActorRef;
}

int AMyActor2::FindRoot(FString s1)
{
  std::string s = std::string(TCHAR_TO_UTF8(*s1));
  std::string names[20] = {"meth", "eth", "prop", "but", "pent", "hex", "hept", "oct", "non", "dec", "undec", "dodec", "tridec", "tetradec", "pentadec", "hexadec", "heptadec", "octadec", "nonadec", "icos"};
  //"���", "��", "����" , "���" , "����" , "����" , "����" , "���" , "���" , "���"
  //std::locale::global(std::locale(""));
  std::vector<int> iters(s.size());
  for (int i = 0; i < 20; i++)
  {
    std::size_t found = s.find(names[i]);
    if (found != std::string::npos)
      iters[static_cast<int>(found)] = i + 1;
  }
  std::sort(iters.rbegin(), iters.rend());

  return iters[0] - iters[1] > 1 ? iters[0] : iters[1];
  //return iters[0];
  //mETHan and ETHan
}

void AMyActor2::RenderAdjacencyList()
{
  std::vector<bool> visited(AdjacencyList.size());
  std::queue<int> q;
  q.push(0);
  while (!q.empty())
  {
    int v = q.front();
    q.pop();
    visited[v] = true;
    for (auto i : AdjacencyList[v])
      if (!visited[i])
        q.push(i);
    FRotator r = {0, 0, 0};
    SpawnObject({20.f + 100 * v, 400.f, 200.f}, r, 8);
  }
}

int AMyActor2::GetAtomIndexByName(FString atom)
{
  for (size_t i = 1; i < atomName.size(); i++)  {
    if (TCHAR_TO_UTF8(*atom) == atomName[i]) return i;
  }
  return -1;
}

std::vector<std::vector<FVector>> positioning = {
    {{0, 0, 0}},                                                                                 // 0
    {{50, 0, 0}},                                                                               // 1
    {{158.67068543265904, 121.75221130618945, 0}, {158.67068543265904, -121.75221130618945, 0}}, // 2
    /* 
    a * sin(0.916298), a * sin(0.654498)
    52.5 deg                  37.5 deg
    angle between those 3 points is 105 deg
    */
    {{142.2, 100.2, 81.65}, {142.2, -100, 81.65}, {142.2, 0, -81.65}}, // 3
    /* 
    a, (a * sqrt(3) + a) / 2, 0
    a, (a * sqrt(3) - a) / 2, 0
    a - (a / (2 * sqrt(3))), a * sqrt(3) / 2, -a * sqrt(6)
    */
    {{100, 50, 50}, {100, 50, -50}, {100, -50, 50}, {100, -50, -50}}};

void AMyActor2::SpawnGraph(int previous, int current, TArray<bool> &visited, TArray<int> &molecule, TArray<AActor*> &elements)
{
  // If it's first atom to spawn
  if (previous == -1)
  {
    FRotator abc = {0, 0, 0};
    elements[0] = SpawnObject({-500.f, 0.f - 700 * nOfGraph, 200.f}, abc, GetAtomIndexByName(UTF8_TO_TCHAR(decodeInfo[0].c_str())));
    previous = 0;
    // Spawn myself
  }
  // If visited
  if (visited[current] == true)
    return;
  visited[current] = true;

  std::set<int> unique;
  for (int i:graph[current]) unique.insert(i);
  int neighborsCount = unique.size() - 1; // -1 because 1 of neighbors is already spawned
  if (!current) neighborsCount += 1;
  // Spawn neighbors
  int j = 0;
  for (int i : graph[current])
  {
    if (visited[i]) continue;
    UE_LOG(LogTemp, Warning, TEXT("about to spawn graph[%i]"), i);
    // Get coords of previous
    FVector coordsPrevious = elements[previous]->GetActorLocation();
    FVector coordsCurrent = elements[current]->GetActorLocation();
    FVector previousToCurrent = {coordsPrevious.X - coordsCurrent.X, coordsPrevious.Y - coordsCurrent.Y, coordsPrevious.Z - coordsCurrent.Z};
    FRotator abc = {0, 0, 0}; // How can I do this?
    UE_LOG(LogTemp, Warning, TEXT("Spawning element x:%i y:%i z:%i"), 
      static_cast<int>(positioning[neighborsCount][j].X),
      static_cast<int>(positioning[neighborsCount][j].Y),
      static_cast<int>(positioning[neighborsCount][j].Z));
    elements[i] = SpawnObject({coordsCurrent.X + positioning[neighborsCount][j].X, coordsCurrent.Y + positioning[neighborsCount][j].Y, coordsCurrent.Z + positioning[neighborsCount][j].Z}, abc, GetAtomIndexByName(UTF8_TO_TCHAR(decodeInfo[i].c_str())));
    SpawnGraph(current, i, visited, molecule, elements);
    j += 1;
  }
}

// Called when the game starts or when spawned
void AMyActor2::BeginPlay()
{
  Super::BeginPlay();
  isStarted = true;
  /*
	int count = 8;//int count = FindRoot(compound);
	//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("a= %i"), count));
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("Добро пожаловать в АД")));
	float prevX = -500.f, prevY = 0.f;
	///Spawning(count, {-500.f, 0.f, 0.f}, angle);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Angle is: %s"), log.ToString()));

	// Try to create H20 list, and then render it all
	AdjacencyList.resize(3);
	AdjacencyList[0] = {1};	   //H
	AdjacencyList[1] = {0, 2}; //O
	AdjacencyList[2] = {1};	   //H
	RenderAdjacencyList();*/
  // need a module which takes H2O, then creates 3 parts, and finally link them up
}

void AMyActor2::Spawning(int count, FVector v, float ang)
{

  GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("compound=%i"), count));
  float prevX = v.X, prevY = v.Y;
  for (int i = 0; i < count; i++)
  {
    float angleCos = std::cos((ang - 180) * PI / 180.0);
    float b = distance * angleCos;
    float a = std::sqrt(std::pow(distance, 2) - std::pow(b, 2));
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Angle is: %s"), log.ToString()));
    //GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("a=%f b=%f angleCos=%f"), a, b, angleCos));
    FRotator r = {0, 0, 0};
    prevX += a;
    prevY += b;
    SpawnObject({prevX + a, prevY + b, 200}, r, i % 10 + 1);
  }

  float angleCos = std::cos((ang - 180) * PI / 180.0);
  float b = distance * angleCos;
  float a = std::sqrt(std::pow(distance, 2) - std::pow(b, 2));
  //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Angle is: %s"), log.ToString()));
  GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Orange, FString::Printf(TEXT("a=%f b=%f angleCos=%f"), a, b, angleCos));
}

void AMyActor2::NewMolecule_Implementation()
{
  UE_LOG(LogTemp, Warning, TEXT("%s"), *GetClass()->GetName());
}

TArray<FVectors> AMyActor2::GetMoleculeGraph()
{
  TArray<FVectors> arr;
  arr.SetNum(graph.size());
  UE_LOG(LogTemp, Warning, TEXT("arr created and resized"));
  for (size_t i = 0; i < graph.size(); i++)
  {
    FVectors f;
    TArray<int> t;
    t.SetNum(graph[i].size());
    for (size_t j = 0; j < graph[i].size(); j++)
    {
      t[j] = graph[i][j];
    };
    f.Vector = t;
    arr[i] = f;
  }
  return arr;
}

TArray<FString> AMyActor2::GetMoleculeDecodeInfo()
{
  TArray<FString> decodeInfoFS;
  decodeInfoFS.SetNum(decodeInfo.size());
  for (size_t i = 0; i < decodeInfo.size(); i++)
    decodeInfoFS[i] = UTF8_TO_TCHAR(decodeInfo[i].c_str());
  return decodeInfoFS;
}

int AMyActor2::GetNOfGraph()
{
  return nOfGraph;
}

static TAutoConsoleVariable<FString> C(
    TEXT("C"),
    "octan",
    TEXT("Well this is just a test debug flag.\n")
        TEXT("<=0: off \n")
            TEXT(">=1: enable debug something\n"),
    ECVF_SetByConsole);

static TAutoConsoleVariable<FString> S(
    TEXT("S"),
    "-",
    TEXT("Well this is just a test debug flag.\n")
        TEXT("<=0: off \n")
            TEXT(">=1: enable debug something\n"),
    ECVF_SetByConsole);

// Called every frame
void AMyActor2::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
  //Spawning(8, { -700.f, -200.f, 200.f });
  std::string moleculeInp(TCHAR_TO_UTF8(*S.GetValueOnGameThread()));
  if (prev != moleculeInp && moleculeInp != "-" && isStarted)
  {
    UE_LOG(LogTemp, Warning, TEXT("Console variable is here"));
    tie(graph, decodeInfo) = ParseMolecule(moleculeInp);
    // NewMolecule();
    TArray<bool> b;
    b.SetNum(graph.size());
    TArray<int> m;
    m.SetNum(graph.size());
    TArray<AActor*> e;
    e.SetNum(graph.size());
    SpawnGraph(-1, 0, b, m, e);
    nOfGraph += 1;
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Orange, FString::Printf(TEXT("decodeInfo.size()=%i"), decodeInfo.size() + 0.f));
    // size_t -> bp -> for each index get TArray
    // BuildMolecule(arr);
    prev = moleculeInp;
  }
  /*int count = 8; //int count = FindRoot(C.GetValueOnGameThread());
	if (prev != compound_string)
	{
		Spawning(count, {-600.f, prevRow * -300.f, 200.f}, 90.f);
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Orange, FString::Printf(TEXT("compound=%f"), count + 0.f));
		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, TEXT("WAT"));
		//������
		prevRow++;
		prev = compound_string;
	}
	
	if (ActorsList.size() > 2)
	{
		circleAngle += DeltaTime / 1.5;
		float circleX = sin(circleAngle) * circleRadius, circleY = cos(circleAngle) * circleRadius;
		FVector resultLocation = {circleFocus.X + circleX, circleFocus.Y + circleY, circleFocus.Z};
		ActorsList[2]->SetActorLocation(resultLocation);
	}*/
}
