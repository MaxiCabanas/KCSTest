# What is this?
This is a job application.  
The objective was to replicate "Invaders" but with a twist, some enemies drops crystals when killed. 
Player can collect and spend those crystals in using abilities.   

## Abilities: An approach using GameplayTasks.
Because the small scope of the project, I decided to build a small ability system on top of the "GameplayTask" 
module of UE5.

### Requirement:
By requirement was the player should be able to use three abilities using three different inputs: 
slow motion, stop the time and reverse the time.  

### Implementation:
#### UGameplayTask_KCSPlayerTimeTask:  
Base class for the rest of abilities. 
It defines all the basic virtual function that the rest of subclasses will extend in functionality.  

#### UGameplayTask_KCSSlowMoTask:  
This ability can slow the time (or make it faster) by the set factor by the set time duration. 
This ability is used reused for the "time stop" ability setting the "SlowMoDilation" parameter to 0.0f

#### UGameplayTask_KCSReverseTimeTask  
This ability interacts with the enemies behavior and the "Timeline system" in order to give the sensation that the time 
is going in reverse for the enemies.

More about the Timeline system later.

### Final notes about the ability system.
There is a flaw in how the base ability is implemented, every time the player activate an ability the ability is instantiated
and activated at the same frame.  
Because the abilities are not used very often this is not big problem but a more correct implementation is to instantiate
the ability at begin play and when the input is pressed call ReadyForActivation or PerformActivation instead. 
Also, EndTask should not destroy the instance of the ability, just reset it state prior to activation.

## Timelines: keyframes and reflection.
A small disclaimer first: My approach for this system was trying to reuse the struct FTimeline provided by the engine, 
since it already handles the use of reflection to animate whatever property is being tracked inside it.

At the end of the day a lot of boilerplate and extra code was required in order to reuse FTimeline, 
because it's not designed to be extended easily.

### Implementation
#### UKCSTimelineComponent
This component is owned by every actor we need to be tracked by the system.  
It contains a TMap that matches a FKCSPropertyRecorderTimeline with a UObject, so we can manipulate properties 
in more than one component instead of only the owner.

Interacting with this component all tracked properties can be recorded, replayed in reverse or forward, etc.

On top of the basic functionality it also handles the specific tracking of the LocalPosition of the component's owner.

#### FKCSPropertyRecorderTimeline
Wrapper struct that matches an array of FKCSRecordableProperty to an UObject and sets up a FTimeline to track them and
record keyframes with the values of those properties.

Contains a series of function for easy manipulation of the FTimeline from outside.
#### FKCSRecordableProperty
This struct represent a property that will be tracked overtime. Because it's intended to be used with FTimeline 
the type of the property must be specified explicitly using one of the values of EKCSRecordedPropertyType, 
currently supporting float and vector.

## GameplayEventSystem: putting the systems in communication.
Very simple system. The class UKCSGameplayTagMessageSubsystem provides a way to register functions to a GameplayTag. 
When an "Event" with any relevant tag is triggered, all the functions listening to that tag are triggered.

Also provides a way to request an event to be triggered, so any other class listening to that request can trigger 
whatever is needed. This feature is very useful to avoid racing conditions.