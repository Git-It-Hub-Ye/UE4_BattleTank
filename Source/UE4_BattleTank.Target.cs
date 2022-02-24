// Copyright 2018 - 2022 Stuart McDonald.

using UnrealBuildTool;
using System.Collections.Generic;

public class UE4_BattleTankTarget : TargetRules
{
	public UE4_BattleTankTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "UE4_BattleTank" } );
	}
}
