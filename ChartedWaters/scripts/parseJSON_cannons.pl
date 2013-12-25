#$name = $ARGV[0];
#if ($name eq '')
#  {die "[No name input]";}
$name = "cannons";
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

	#ID	Type	Name	base price	durability	damage(per 2)	penetration(= base / 5)	max_range	speed	explosion	reload_speed	description
	$myline =~ /($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)/;
	$ID = $1;
	$type = $2;
	$name = $3;
	$price = $4;
	$durability = $5;
	$damage = $6;
	$penetration = $7;
	$max_range = $8;
	$min_range = $9;
	$speed = $10;
	$explosion = $11;
	$reload_speed = $12;
	$desc = $13;

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
	print $RAWOUT "\t\t\"damage\": $damage,\n";
	print $RAWOUT "\t\t\"penetration\": $penetration,\n";	
	print $RAWOUT "\t\t\"max_range\": $max_range,\n";
	print $RAWOUT "\t\t\"min_range\": $min_range,\n";
	print $RAWOUT "\t\t\"speed\": $speed,\n";
	print $RAWOUT "\t\t\"explosion\": $explosion,\n";
	print $RAWOUT "\t\t\"reload_speed\": $reload_speed\n";

	unless ($desc eq '')
		{print $RAWOUT ",\n\t\t\"desc\": \"$desc\"";}
	print $RAWOUT "\n\t}";
}

print $RAWOUT "\n]"
