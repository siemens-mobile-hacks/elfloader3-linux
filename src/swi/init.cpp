#include "src/swi/init.h"
#include "src/swi/csm.h"
#include "src/swi/gui.h"
#include "src/swi/proc/helper.h"
#include "src/swi/proc/mmi.h"

#define MMI_PGROUP_ID 0x42
#define RAP_PGROUP_ID 0x44

void OS_Init() {
	PGROUP mmi_pgroup = {};
	mmi_pgroup.id = MMI_PGROUP_ID;
	mmi_pgroup.name = (char *) "T_MMIMN";
	mmi_pgroup.prio = 0x5B;
	CreateICL(&mmi_pgroup);

	PGROUP helper_pgroup = {};
	helper_pgroup.id = RAP_PGROUP_ID;
	helper_pgroup.name = (char *) "T_RAP";
	mmi_pgroup.prio = 0x6E;
	CreateICL(&helper_pgroup);

	GUI_Init();
	CSM_Init();
	Helper_Init();
	MMI_Init();
}
