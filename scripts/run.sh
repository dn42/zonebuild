#!/bin/bash
#
# Read config first, then copy your settings to config.user 
#
# Build options:
#   root
#   zone
#   arpa
#
# Define arpa build tiers in scripts/config

BASE_PATH=`dirname ${0}`

. "${BASE_PATH}/config" || exit 2
. "${BASE_PATH}/common" || exit 2

USAGE_STR="USAGE: ./`basename ${0}` <options> .."

[[ -z "${@}" ]] && {
	print_usage_and_exit	
}


rm -f ${OUT_PATH}/tier[0-9]/*.db ${OUT_PATH}/tier[0-9]/*.conf

[[ "${@}" = *nosync* ]] && {
	PULL_BEFORE_BUILD=0
}

for hook in "${PRE_BUILD_HOOKS[@]}"; do
	eval "${hook}"
done

[[ "${@}" = *root* ]] && {
	echo "${0}: [T0] processing tier0.."	
	${BASE_PATH}/build_tier0.sh dn42 || {
		echo "${0}: tier 0 failed: ${?}"
	}
}

[[ "${@}" = *zone* ]] && {
	for item in ${TIER1_ZONES[@]}; do	
		echo "${0}: [T1] processing '${item}'"	
		${BASE_PATH}/build_tier1.sh ${item} || {
		echo "${0}: tier 1 failed: ${?}"
	}
	done
}

[[ "${@}" = *arpa* ]] && {
	[[ "${ARPA_TIERS}" = *1*  ]] && {
		for item in ${ARPA_ZONES[@]}; do
			echo "${0}: [T1-A]: processing ${item}"
			${BASE_PATH}/build_tier1_arpa.sh ${item} 0 1 || {
				echo "${0}: tier 1 arpa failed: ${?}"
			}			
		done
		[ ${TIER1_IPV6} -eq 1 ] && {
			export SUBNETTR_CONTACT=${CONTACT_EMAIL}
			export SUBNETTR_PERSON=${PERSON_HANDLE}
			export SUBNETTR_PRIMARY=${SERVER_NAME_TIER1_ARPA}
			export SUBNETTR_REVISION=$REVISION
			
			run_subnettr || {
				echo ${0}: subnettr failed
				exit 2;
			}
			
			mkdir -p ${OUT_PATH}/ipv6
			
			for zone in ${ARPA_IPV6_ZONES[@]}; do
				cu_add_master_zone ${OUT_PATH}/tier1/named.conf "${zone}.ip6.arpa" ${OUT_PATH}/ipv6/db.${zone}.ip6.arpa
			done
		}
	}
	[[ "${ARPA_TIERS}" = *2*  ]] && {
		for item in ${ARPA_ZONES[@]}; do
			echo "${0}: [T2-A]: processing ${item}"
			${BASE_PATH}/build_tier2_arpa.sh ${item}	
			${BASE_PATH}/build_tier1_arpa.sh ${item} 0 1 || {
				echo "${0}: tier 2 arpa failed: ${?}"
			} 
		done
	}
}



for hook in "${POST_BUILD_HOOKS[@]}"; do
	eval "${hook}"
done
