# Linker script for ARM COFF.
echo "Yow in scripttempl/armcoff.sc"
# Based on i386coff.sc by Ian Taylor <ian@cygnus.com>.
test -z "$ENTRY" && ENTRY=_start
if test -z "${DATA_ADDR}"; then
  if test "$LD_FLAG" = "N" || test "$LD_FLAG" = "n"; then
    DATA_ADDR=.
  fi
fi

# These are substituted in as variables in order to get '}' in a shell
# conditional expansion.
CTOR='.ctor : {
    *(SORT(.ctors.*))
    *(.ctor)
  }'
DTOR='.dtor : {
    *(SORT(.dtors.*))
    *(.dtor)
  }'
if test "${RELOCATING}"; then
  R_IDATA234='
    SORT(*)(.idata$2)
    SORT(*)(.idata$3)
    /* These zeroes mark the end of the import list.  */
    LONG (0); LONG (0); LONG (0); LONG (0); LONG (0);
    SORT(*)(.idata$4)'
  R_IDATA5='SORT(*)(.idata$5)'
  R_IDATA67='
    SORT(*)(.idata$6)
    SORT(*)(.idata$7)'
else
  R_IDATA234=
  R_IDATA5=
  R_IDATA67=
fi

cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}", "${BIG_OUTPUT_FORMAT}", "${LITTLE_OUTPUT_FORMAT}")
${LIB_SEARCH_DIRS}

${RELOCATING+ENTRY (${ENTRY})}

SECTIONS
{
  /* We start at 0x8000 because gdb assumes it (see FRAME_CHAIN).
     This is an artifact of the ARM Demon monitor using the bottom 32k
     as workspace (shared with the FP instruction emulator if
     present): */
  .text ${RELOCATING+ 0x8000} : {
    *(.init)
    *(.text*)
    *(.glue_7t)
    *(.glue_7)
    *(.rdata)
    ${CONSTRUCTING+ ___CTOR_LIST__ = .; __CTOR_LIST__ = . ; 
			LONG (-1); *(.ctors); *(.ctor); LONG (0); }
    ${CONSTRUCTING+ ___DTOR_LIST__ = .; __DTOR_LIST__ = . ; 
			LONG (-1); *(.dtors); *(.dtor);  LONG (0); }
    *(.fini)
    ${RELOCATING+ etext  =  .;}
    ${RELOCATING+ _etext =  .;}
  }
  .data ${RELOCATING+${DATA_ADDR-0x40000 + (ALIGN(0x8) & 0xfffc0fff)}} : {
    ${RELOCATING+  __data_start__ = . ;}
    *(.data*)
        
    ${RELOCATING+*(.gcc_exc*)}
    ${RELOCATING+___EH_FRAME_BEGIN__ = . ;}
    ${RELOCATING+*(.eh_fram*)}
    ${RELOCATING+___EH_FRAME_END__ = . ;}
    ${RELOCATING+LONG(0);}
    
    ${RELOCATING+ __data_end__ = . ;}
    ${RELOCATING+ edata  =  .;}
    ${RELOCATING+ _edata  =  .;}
  }
  ${CONSTRUCTING+${RELOCATING-$CTOR}}
  ${CONSTRUCTING+${RELOCATING-$DTOR}}
  .idata ${RELOCATING+BLOCK(__section_alignment__)} :
  {
    /* This cannot currently be handled with grouped sections.
	See pep.em:sort_sections.  */
	${R_IDATA234}
	${RELOCATING+__idata5_start__ = .;}
    ${R_IDATA5}
	${RELOCATING+__idata5_end__ = .;}
    ${R_IDATA67}
  }
  .bss ${RELOCATING+ ALIGN(0x8)} :
  { 					
    ${RELOCATING+ __bss_start__ = . ;}
    *(.bss)
    *(COMMON)
    ${RELOCATING+ __bss_end__ = . ;}
  }

  ${RELOCATING+ end = .;}
  ${RELOCATING+ _end = .;}
  ${RELOCATING+ __end__ = .;}

  .stab  0 ${RELOCATING+(NOLOAD)} : 
  {
    [ .stab ]
  }
  .stabstr  0 ${RELOCATING+(NOLOAD)} :
  {
    [ .stabstr ]
  }
}
EOF
