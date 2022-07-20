//
// Minimal Client for explore network of engine.
//
//

#pragma once

#include "CoreMinimal.h"

#include "Engine/NetConnection.h"
#include "Engine/PendingNetGame.h"

#include "MinimalClient.generated.h"


NETWORKTESTER_API DECLARE_LOG_CATEGORY_EXTERN(LogNetworkTester, Log, All);

// Delegates

/**
 * Delegate for marking the minimal client as having connected fully
 */
DECLARE_DELEGATE(FOnMinClientConnected);

/**
 * Delegate for passing back a network connection failure
 *
 * @param FailureType	The reason for the net failure
 * @param ErrorString	More detailed error information
 */
DECLARE_DELEGATE_TwoParams(FOnMinClientNetworkFailure, ENetworkFailure::Type /*FailureType*/, const FString& /*ErrorString*/);

/**
 * Delegate for hooking the control channel's 'ReceivedBunch' call
 *
 * @param Bunch		The received bunch
 */
DECLARE_DELEGATE_OneParam(FOnMinClientReceivedControlBunch, FInBunch&/* Bunch*/);


/**
 * Delegate for hooking the net connections 'ReceivedRawPacket'
 *
 * @param Data		The data received
 * @param Count		The number of bytes received
 */
DECLARE_DELEGATE_TwoParams(FOnMinClientReceivedRawPacket, void* /*Data*/, int32& /*Count*/);

/**
 * Delegate for notifying on (and optionally blocking) replicated actor creation
 *
 * @param ActorClass	The class of the actor being replicated
 * @param bActorChannel	Whether or not this actor creation is from an actor channel
 * @param bBlockActor	Whether or not to block creation of the actor (defaults to true)
 */
DECLARE_DELEGATE_ThreeParams(FOnMinClientRepActorSpawn, UClass* /*ActorClass*/, bool /*bActorChannel*/, bool& /*bBlockActor*/);

/**
 * Delegate for notifying AFTER an actor channel actor has been created
 *
 * @param ActorChannel	The actor channel associated with the actor
 * @param Actor			The actor that has just been created
 */
DECLARE_DELEGATE_TwoParams(FOnMinClientNetActor, UActorChannel* /*ActorChannel*/, AActor* /*Actor*/);

/**
 * Delegate for hooking the HandlerClientPlayer event
 *
 * @param PC			The PlayerController being initialized with the net connection
 * @param Connection	The net connection the player is being initialized with
 */
DECLARE_DELEGATE_TwoParams(FOnHandleClientPlayer, APlayerController* /*PC*/, UNetConnection* /*Connection*/);

/**
 * Delegate for notifying of failure during call of SendRPCChecked
 */
DECLARE_DELEGATE(FOnRPCFailure);

/* on message delegate */
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnReceiveMessage, const FString &InText, UNetConnection* /*Connection*/);


// base class for implementing a bare bones/stripped-down game client or listened server.
UCLASS()
class NETWORKTESTER_API UMinimalClient : public UObject, public FNetworkNotify, public FTickableGameObject
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	virtual bool IsTickableInEditor() const override
	{
		return true;
	}

	// Listen as a server
	bool Listen(const FString& InServerAddr, uint16 InPort);

	// connect to server as a client
	bool Connect(const FString& InServerAddr, uint16 InPort);

	// Disconnects and cleans up the minmal client.
	void Cleanup();

	/**
	* Resets the net connection timeout
	*
	* @param Duration	The duration which the timeout reset should last
	*/
	void ResetConnTimeout(float Duration);

	void SendInitialJoin();

	void SendText(FString& InText);

	FOnReceiveMessage  ReceiveMessageDel;
protected:
	// create world
	UWorld* CreateWorld();
	void CleanupWorld(UWorld *InWorld);

	// create net driver
	UNetDriver* CreateNetDriver();
	void CleanupNetDriver(UNetDriver* InDriver);

	// FNetworkNotify
protected:
	virtual EAcceptConnection::Type NotifyAcceptingConnection() override
	{
		return EAcceptConnection::Accept;
	}

	virtual void NotifyAcceptedConnection(UNetConnection* Connection) override;

	virtual bool NotifyAcceptingChannel(UChannel* Channel) override;

	virtual void NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, FInBunch& Bunch) override
	{
	}

private:
	/** The amount of time (in seconds) before the connection should timeout */
	uint32 Timeout;

	/** Stores a reference to the created empty world, for execution and later cleanup */
	UWorld* UnitWorld;

	/** Stores a reference to the created unit test net driver, for execution and later cleanup */
	UNetDriver* UnitNetDriver;
};
