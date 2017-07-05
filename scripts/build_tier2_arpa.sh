#!/usr/bin/env bash
#@VERSION:0
#@REVISION:48

if [ -n "${2}" ]; then
	ucfile="${2}"
else
	ucfile="config.user"
fi


. `dirname ${0}`/config || exit 2
. `dirname ${0}`/common || exit 2

USAGE_STR="USAGE: ./`basename ${0}` <root zone path>"

[ -z ${1} ] && {
	print_usage_and_exit	
}

! [ -d "${OUT_PATH}/tier2" ] && 
	mkdir -p ${OUT_PATH}/tier2 

RFC2317_ALL=(`${ZBUILD} -build inetnum --path ${REGISTRY_PATH}/inetnum --root ${1} \
				-lom "rfc2317 = 1 && ([p:rfc2317]) != 1" -lom "([p:nscount]) = 0 || nslevel = 1" \
	 			--noshadow -print '{?Q:({?C:1:startip\}.\{?C:2:startip\}.\{?C:3:startip\})}{:n}' | sort -u`)

for i in ${RFC2317_ALL[@]}; do
	ziname="${i}.in-addr"
	zname="${ziname}.arpa"

	echo "\$TTL ${TIER1_TTL}
${zname}.		IN 	SOA		${SERVER_NAME_TIER2_ARPA}. ${CONTACT_EMAIL}. (`date +%s` 14400 3600 1200 7200)
`generate_version_txt ${zname}`
\$ORIGIN	${zname}." >> ${OUT_PATH}/tier2/${ziname}.db
	
	generate_forward_zone ${REGISTRY_PATH}/dns/dn42-servers.dn42 ${zname} noglue>>  $OUT_PATH/tier2/${ziname}.db 
	
	${ZBUILD} -build inetnum --path ${REGISTRY_PATH}/inetnum --nons --noshadow \
		-print '$GENERATE {?m:startip[0]}-{?m:endip[0]} $ CNAME $.{?m:startip[0]}/{pfxsize}{:n}' --root ${1} \
		-lom "rfc2317 = 1 && ([p:rfc2317]) != 1 && nscount > 0" -l: "(?Q:(\{?m:startip[1]\}.\{?m:startip[2]\}.\{?m:startip[3]\}))" \
		-regex "^${i}$" \
		>> $OUT_PATH/tier2/${ziname}.db
	
	${ZBUILD} -build inetnum --path ${REGISTRY_PATH}/inetnum -print '{?m:startip[0]}/{pfxsize} IN NS {nserver}.{:n}' --root ${1} \
		-lom "rfc2317 = 1 && ([p:rfc2317]) != 1 && nscount > 0" -l: "(?Q:(\{?m:startip[1]\}.\{?m:startip[2]\}.\{?m:startip[3]\}))" \
		-regex "^${i}$" \
		>> $OUT_PATH/tier2/${ziname}.db

	cu_add_master_zone ${OUT_PATH}/tier2/named.conf ${ziname}.arpa ${OUT_PATH}/tier2/${ziname}.db || {
	! [ $? -eq 1 ] && {
		echo "${ziname}: configuration update failed : $? | ${OUT_PATH}"
		exit 2
	}
}

done

generate_soa ${SERVER_NAME_TIER2_ARPA} dn42-servers.dn42 ${TIER0_TTL} > ${OUT_PATH}/tier2/dn42-servers.dn42.db 
generate_forward_zone ${REGISTRY_PATH}/dns/dn42-servers.dn42 dn42-servers.dn42 >> ${OUT_PATH}/tier2/dn42-servers.dn42.db 
cu_add_master_zone ${OUT_PATH}/tier2/named.conf dn42-servers.dn42 ${OUT_PATH}/tier2/dn42-servers.dn42.db

exit 0