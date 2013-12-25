$name = $ARGV[0];
if ($name eq '')
  {die "[No name input]";}
$rawmatin = $name . ".protojson";
$rawmatout = $name . ".json";

use autodie;

open ($RAWIN, "<", $rawmatin);
open ($RAWOUT, ">", $rawmatout);

print $RAWOUT "[";
$started = 0;

for $line (<$RAWIN>)
{
	$myline = $line;

	chomp ($myline);
	$text = '[^\t]';

	#ID type name price durability desc [1] [2] [3]
	$myline =~ /($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)/;
	$ID = $1;
	$type = $2;
	$name = $3;
	$price = $4;
	$durability = $5;
	$desc = $6;
	$info[0] = $7;
	$info[1] = $8;
	$info[2] = $9;

	if ($ID eq 'ID')
		{next;}

	if ($started == 1)
		{print $RAWOUT ",\n";}
	else
	{
		print $RAWOUT "\n";
		$started = 1;
	}	
	print $RAWOUT "\t{\n\t\t";
	print $RAWOUT "\"ID\": \"$ID\",\n";
	print $RAWOUT "\t\t\"type\": \"$type\",\n";
	print $RAWOUT "\t\t\"name\": \"$name\",\n";
	print $RAWOUT "\t\t\"price\": $price,\n";
	print $RAWOUT "\t\t\"durability\": $durability,\n";

	if ($type eq 'sail')
	{
	if ($info[0] eq '') { $info[0] = 0; }
	if ($info[1] eq '') { $info[1] = 0; }
	if ($info[2] eq '') { $info[2] = 0; }
	print $RAWOUT "\t\t\"lateen\": $info[0],\n";
	print $RAWOUT "\t\t\"square\": $info[1],\n";
	print $RAWOUT "\t\t\"turning\": $info[2]";
	}
	elsif ($type eq 'plate')
	{
	print $RAWOUT "\t\t\"armor\": $info[0],\n";
	print $RAWOUT "\t\t\"speed\": $info[1]";
	}
	elsif ($type eq 'statue')
	{
	if ($info[0] eq '') { $info[0] = 0; }
	if ($info[1] eq '') { $info[1] = 0; }
	if ($info[2] eq '') { $info[2] = 0; }
	print $RAWOUT "\t\t\"protection\": $info[0],\n";
	print $RAWOUT "\t\t\"healing\": $info[1],\n";
	print $RAWOUT "\t\t\"dodging\": $info[2]";
	}

	unless ($desc eq '')
		{print $RAWOUT ",\n\t\t\"desc\": \"$desc\"";}

	print $RAWOUT "\n\t}";
}

print $RAWOUT "\n]"
