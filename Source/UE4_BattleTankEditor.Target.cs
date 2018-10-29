// Copyright 2018 Stuart McDonald.

using UnrealBuildTool;
using System.Collections.Generic;

public class UE4_BattleTankEditorTarget : TargetRules
{
	public UE4_BattleTankEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "UE4_BattleTank" } );
	}
}
