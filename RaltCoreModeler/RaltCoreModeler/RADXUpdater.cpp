#include "RADXUpdater.h"

void RADXUpdater::Update()
{
	static INT timeStart = GetTickCount();
	INT dwTimeCur = GetTickCount();
	INT timeUpdate = dwTimeCur - timeStart;
	timeStart = dwTimeCur;

	DRCA()->GetRaltID()->UpdateInputDevice(timeUpdate);
	//DRCA()->GetInterface()->Update(timeUpdate);
}
