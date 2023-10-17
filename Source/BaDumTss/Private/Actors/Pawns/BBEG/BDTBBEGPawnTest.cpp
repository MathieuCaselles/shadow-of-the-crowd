#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#include "Actors/Pawns/BBEG/BDTBBEGPawn.h"
#include "Tests/AutomationCommon.h"

static constexpr int TestFlags = (
	  EAutomationTestFlags::EditorContext
	| EAutomationTestFlags::CommandletContext
	| EAutomationTestFlags::ClientContext
	| EAutomationTestFlags::ProductFilter );


// EditorContext defines that we want to be able to run this test in the Editor
// ProductFilter is for defining how long the test will take to run
// For more information check AutomationTest.h

// Asynchronous function to create a hero
DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER( FBDTSetupBBEG, ABDTBBEGPawn*, BBEG );
bool FBDTSetupBBEG::Update()
{
	BBEG = NewObject<ABDTBBEGPawn>();
	return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER( FBBEGTestGoldusAt0Secs, FAutomationTestBase*, Test, ABDTBBEGPawn*, BBEG );
bool FBBEGTestGoldusAt0Secs::Update()
{
	Test->TestEqual( "BBEG Should have 10 goldus on start", BBEG->GetGoldus(), 10 );
	return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER( FBBEGTestGoldusAt10Secs, FAutomationTestBase*, Test, ABDTBBEGPawn*, BBEG );
bool FBBEGTestGoldusAt10Secs::Update()
{
	Test->TestEqual( "BBEG Should have 13 goldus at 10 secs", BBEG->GetGoldus(), 13 );
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST( FBBEGPawnTest, "BadumTesst.BBEGPawn.Goldus", TestFlags )
bool FBBEGPawnTest::RunTest( const FString& Parameters )
	{
		// Wait for 1.5s, then test
		ABDTBBEGPawn* BBEG = nullptr;
		ADD_LATENT_AUTOMATION_COMMAND( FBDTSetupBBEG( BBEG ) );
		ADD_LATENT_AUTOMATION_COMMAND( FWaitLatentCommand( 1.5f ) );
		ADD_LATENT_AUTOMATION_COMMAND( FBBEGTestGoldusAt0Secs( this, BBEG ) );
		ADD_LATENT_AUTOMATION_COMMAND( FWaitLatentCommand( 10.f ) );
		ADD_LATENT_AUTOMATION_COMMAND( FBBEGTestGoldusAt10Secs( this, BBEG ) );
		return true;
	}