#include "system.h"

#include "rpmlib.h"

#include "depends.h"
#include "misc.h"

char * rpmProblemString(rpmProblem prob) {
    char * name, * version, * release;
    char * buf;
    char * altName, * altVersion, * altRelease;

    headerGetEntry(prob.h, RPMTAG_NAME, NULL, (void **) &name, NULL);
    headerGetEntry(prob.h, RPMTAG_VERSION, NULL, (void **) &version, NULL);
    headerGetEntry(prob.h, RPMTAG_RELEASE, NULL, (void **) &release, NULL);

    if (prob.altH) {
	headerGetEntry(prob.altH, RPMTAG_NAME, NULL, (void **) &altName, NULL);
	headerGetEntry(prob.altH, RPMTAG_VERSION, NULL, (void **) &altVersion, 
		       NULL);
	headerGetEntry(prob.altH, RPMTAG_RELEASE, NULL, (void **) &altRelease, 
		       NULL);
    }

    buf = malloc(strlen(name) + strlen(version) + strlen(release) + 400);

    switch (prob.type) {
      case RPMPROB_BADOS:
	sprintf(buf, _("package %s-%s-%s is for a different operating system"), 
		name, version, release);
	break;
	
      case RPMPROB_BADARCH:
	sprintf(buf, _("package %s-%s-%s is for a different architecture"), 
		name, version, release);
	break;

      case RPMPROB_PKG_INSTALLED:
	sprintf(buf, _("package %s-%s-%s is already installed"),
		name, version, release);
	break;

      case RPMPROB_BADRELOCATE:
	sprintf(buf, _("path %s is not relocateable for package %s-%s-%s"),
		prob.str1, name, version, release);
	break;

      case RPMPROB_NEW_FILE_CONFLICT:
	sprintf(buf, _("file %s conflicts between attemped installs of "
		       "%s-%s-%s and %s-%s-%s"), prob.str1, name, version, 
		release, altName, altVersion, altRelease);
	break;

      case RPMPROB_FILE_CONFLICT:
	sprintf(buf, _("file %s from install of %s-%s-%s conflicts with "
		       "file from package %s-%s-%s"), prob.str1, name, version, 
		release, altName, altVersion, altRelease);
	break;

      default:
	sprintf(buf, _("unknown error %d encountered while manipulating "
		"package %s-%s-%s"), prob.type, name, version, release);
	break;
    }

    return buf;
}

void rpmProblemSetFilter(rpmProblemSet ps, int flags) {
    int i;
    int flag;

    for (i = 0; i < ps->numProblems; i++) {
	switch (ps->probs[i].type) {
	  case RPMPROB_BADOS:
	    flag = RPMPROB_FILTER_IGNOREOS; break;
	  case RPMPROB_BADARCH:
	    flag = RPMPROB_FILTER_IGNOREARCH; break;
	  case RPMPROB_PKG_INSTALLED:
	    flag = RPMPROB_FILTER_REPLACEPKG; break;
	  case RPMPROB_BADRELOCATE:
	    flag = RPMPROB_FILTER_FORCERELOCATE; break;
	  case RPMPROB_NEW_FILE_CONFLICT:
	    flag = RPMPROB_FILTER_REPLACENEWFILES; break;
	  case RPMPROB_FILE_CONFLICT:
	    flag = RPMPROB_FILTER_REPLACEOLDFILES; break;
	  default:
	    flag = 0;
	}

	ps->probs[i].ignoreProblem = !!(flags & flag);
    }
}
