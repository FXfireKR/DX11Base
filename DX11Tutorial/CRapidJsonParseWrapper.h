#pragma once

class CRapidJsonParsorWrapper
{
public:
	static bool ReadFloat(IN const rapidjson::Value& value, OUT float& fOut)
	{
		if (value.IsNumber())
			return false;
		fOut = value.GetFloat();
		return true;
	}

	static bool ReadVector3(IN const rapidjson::Value& arr, float fOut[3])
	{
		if (!arr.IsArray() || arr.Size() < 3) return false;
		for (rapidjson::SizeType i = 0; i < 3; ++i)
		{
			if (!arr[i].IsNumber()) return false;
			fOut[i] = arr[i].GetFloat();
		}
		return true;
	}

	static bool ReadUV4(IN const rapidjson::Value& arr, float fOut[4])
	{
		if (!arr.IsArray() || arr.Size() < 4) return false;
		for (rapidjson::SizeType i = 0; i < 4; ++i)
		{
			if (!arr[i].IsNumber()) return false;
			fOut[i] = arr[i].GetFloat();
		}
		return true;
	}
};