//
//  MinecraftTranslate.h
//  mac
//
//  Copyright © 2020 Ravbug. All rights reserved.
//

#pragma once
#include "OperationBase.h"

#define mc_symbols "%s","%1$s", "%2$s", "%3$s", "%4$s","%%", "%d", "\n", "+", "-", "="

class MinecraftSplitter : public SplitterBase{
	valuetableptr split_file(std::ifstream&) override;

public:
	MinecraftSplitter() {
		symbols = { mc_symbols };
	}
};

class MinecraftJoiner : public JoinerBase {
	void write_combined(valuetableptr table) override;
	
public:
	MinecraftJoiner(){
		symbols = { mc_symbols };
	}
};
