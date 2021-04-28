// Fill out your copyright notice in the Description page of Project Settings.

#include "MyActor2.h"
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
#include <queue>

std::string prev = "";
int prevRow = 2;
std::vector<AActor *> ActorsList;
std::vector<std::vector<int>> AdjacencyList;

FVector circleFocus = {0.f, 0.f, 20.f};
int circleRadius = 100;
float circleAngle = 0;

std::vector<FString> elementClassPath = {
    "",
    "/Game/Blueprints/Hydrogen.Hydrogen",
    "/Game/Blueprints/Helium.Helium",
    "/Game/Blueprints/Lithium.Lithium",
    "/Game/Blueprints/Beryllium.Beryllium",
    "/Game/Blueprints/Boron.Boron",
    "/Game/Blueprints/Carbon.Carbon",
    "/Game/Blueprints/Nitrogen.Nitrogen",
    "/Game/Blueprints/Oxygen.Oxygen",
    "/Game/Blueprints/Fluorine.Fluorine",
    "/Game/Blueprints/Neon.Neon"

};

std::vector<FString> elementMaterialPath = {
    "",
    "Material'/Game/Collections/MHydrogen.MHydrogen'",
    "Material'/Game/Collections/Helium.Helium'",
    "Material'/Game/Collections/MLithium.MLithium'",
    "/Game/Collections/MBeryllium.MBeryllium",
    "/Game/Blueprints/Boron.Boron",
    "/Game/Blueprints/Carbon.Carbon",
    "/Game/Blueprints/Nitrogen.Nitrogen",
    "/Game/Blueprints/Oxygen.Oxygen",
    "/Game/Blueprints/Fluorine.Fluorine",
    "/Game/Blueprints/Neon.Neon"

};

AMyActor2::AMyActor2()
{
  //int log = std::stoi(names[0]);
  //FString HappyString(names[0].c_str());
  //GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("a= %s"), HappyString));
  prev = TCHAR_TO_UTF8(*compound);
}

void AMyActor2::SpawnObject(FVector Loc, FRotator Rot, int elementIndex)
{
  BPEvent_TestCall(1);
  FActorSpawnParameters SpawnParams;
  // TODO: create an array with classes paths,
  // and then choose from them a name by an periodic el number
  UObject *SpawnActor = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, *elementClassPath[elementIndex]));
  UBlueprint *GeneratedBP = Cast<UBlueprint>(SpawnActor);
  if (!SpawnActor)
  {
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("CANT FIND OBJECT TO SPAWN")));
    return;
  }

  UClass *SpawnClass = SpawnActor->StaticClass();
  if (SpawnClass == NULL)
  {
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("CLASS == NULL")));
    return;
  }
  AActor *SpawnedActorRef = GetWorld()->SpawnActor<AActor>(GeneratedBP->GeneratedClass, {Loc.X, Loc.Y, Loc.Z}, Rot, SpawnParams);
  BPEvent_TestCall(2);
  ActorsList.push_back(SpawnedActorRef);
  if (ActorsList.size() == 21)
    circleFocus = SpawnedActorRef->GetActorLocation();
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

// Called when the game starts or when spawned
void AMyActor2::BeginPlay()
{
  Super::BeginPlay();
  /*//UE_LOG(LogTemp, Warning, TEXT("I just started running"));

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
    SpawnObject({prevX + a, prevY + b, 200}, r, i % 3 + 2);
  }

  float angleCos = std::cos((ang - 180) * PI / 180.0);
  float b = distance * angleCos;
  float a = std::sqrt(std::pow(distance, 2) - std::pow(b, 2));
  //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Angle is: %s"), log.ToString()));
  GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Orange, FString::Printf(TEXT("a=%f b=%f angleCos=%f"), a, b, angleCos));
}

static TAutoConsoleVariable<FString> C(
    TEXT("C"),
    "octan",
    TEXT("Well this is just a test debug flag.\n")
        TEXT("<=0: off \n")
            TEXT(">=1: enable debug something\n"),
    ECVF_SetByConsole);

// Called every frame
/*void AMyActor2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Spawning(8, { -700.f, -200.f, 200.f });
	std::string compound_string(TCHAR_TO_UTF8(*C.GetValueOnGameThread()));
	int count = 8; //int count = FindRoot(C.GetValueOnGameThread());
	if (prev != compound_string)
	{
		Spawning(count, {-600.f, prevRow * -300.f, 200.f}, 90.f);
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Orange, FString::Printf(TEXT("compound=%f"), count + 0.f));
		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, TEXT("WAT"));
		//������
		prevRow++;
		prev = compound_string;
	}
	if (ActorsList.size() > 40)
	{
		circleAngle += DeltaTime;
		float circleX = sin(circleAngle) * circleRadius, circleY = cos(circleAngle) * circleRadius;
		FVector resultLocation = {circleFocus.X + circleX, circleFocus.Y + circleY, circleFocus.Z};
		//ActorsList[40]->SetActorLocation(resultLocation);
	}
	
}
*/