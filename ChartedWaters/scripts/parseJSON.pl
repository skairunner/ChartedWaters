$name = $ARGV[0];
if ($name eq '')
  {die "[No name input]";}
$rawmatin = $name . ".protojson";
$rawmatout = $name . ".json";

#$rawmatout = "rawmaterials.json";
#$rawmatin = "rawmat.protojson";

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

	$myline =~ /($text*)\t($text*)\t($text*)\t($text*)\t*($text*)/;
	$cat = $1;
	$type = $2;
	$name = $3;
	$price = $4;
	$desc = $5;

	if ($cat eq '' or $type eq '' or $name eq '' or $price eq '')
		{next;}

	$prefix = $cat;
	$prefix = lc($prefix);
	$ID = $name;
	$ID = lc($ID);
	$ID = $prefix . "_" . $ID;
	$ID =~ s/ //g;

	if ($started == 1)
		{print $RAWOUT ",\n";}
	else
	{
		print $RAWOUT "\n";
		$started = 1;
	}	
	print $RAWOUT "\t{\n\t\t";
	print $RAWOUT "\"ID\": \"$ID\",\n";
	print $RAWOUT "\t\t\"category\": \"$cat\",\n";
	print $RAWOUT "\t\t\"type\": \"$type\",\n";
	print $RAWOUT "\t\t\"name\": \"$name\",\n";
	print $RAWOUT "\t\t\"base price\": $price";

	unless ($desc eq '')
		{print $RAWOUT ",\n\t\t\"desc\": \"$desc\"";}
	print $RAWOUT "\n\t}";
}

print $RAWOUT "\n]"
