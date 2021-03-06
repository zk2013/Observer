#include "Includes.h"

#include "../Log/Log.h"

#include "../Util/ResourceList.h"

_Use_decl_annotations_
NTSTATUS RegistryFilterUnload(
	PVOID pContext
)
{
	NTSTATUS Status;
	PREGISTRY_FILTER_CONTEXT Context = (PREGISTRY_FILTER_CONTEXT)pContext;

	if (Context == NULL)
	{
		//Nothing to do...
		return STATUS_SUCCESS;
	}

	Status = CmUnRegisterCallback(Context->FilterContextCookie);

	WLockResourceList(&RegistryFilterRuleList);
	while (!IsListEmpty(&RegistryFilterRuleList.ListEntry))
	{
		PREGISTRY_FILTER_RULE_ENTRY CurrentEntry = CONTAINING_RECORD(
			RemoveHeadList(&RegistryFilterRuleList.ListEntry),
			REGISTRY_FILTER_RULE_ENTRY, 
			ListEntry);

		if (InterlockedDecrement(&CurrentEntry->Refcount) == 0)
		{
			REGISTRY_FILTER_FREE(CurrentEntry);
		}

	}

	WUnlockResourceList(&RegistryFilterRuleList);

	ExDeleteResourceLite(&RegistryFilterRuleList.Resource);

	REGISTRY_FILTER_FREE(Context);


	return Status;
}