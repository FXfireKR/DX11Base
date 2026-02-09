#pragma once
#include "CPipeline.h"

class CPipelineManager
{
public:
	CPipelineManager() {
		wow.reserve(10);
	}
	~CPipelineManager() = default;



private:
	sparse_set<CPipeline> wow;
};