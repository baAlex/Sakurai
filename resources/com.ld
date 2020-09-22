OUTPUT_FORMAT(binary)
SECTIONS
{
	. = 0x0100;

	.sakuraiboot :
	{
		*(.sakuraiboot*)
	}
	.text :
	{
		*(.text);
	}
	.data :
	{
		*(.data);
		*(.bss);
		*(.rodata);
	}
}
