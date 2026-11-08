SECTIONS
{
    . = 0x00000000;
    .vector_table : {*(.vector_table)} /* Interrupt vector table */
    . = 0x00000008;
    .text : {*(.text)} /* code: functions etc */
    .data : {*(.data)} /* initialised data: inited global or static vars */
    .bss : {*(.bss)} /* uninitialised data: zero inited or uinited global variables */
}