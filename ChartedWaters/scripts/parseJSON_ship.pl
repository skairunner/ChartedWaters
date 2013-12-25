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

	#ID name size price specialization max_storage max_cargo max_sailor max_cannon min_sailor lateen_sail square_sail wave_resistance turning base_armor max_durability desc
	$myline =~ /($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t($text*)\t*($text*)/;
	$ID = $1;
	$name = $2;
	$size = $3;
	$price = $4;
	$specialization = $5;
	$max_storage = $6;
	$max_cargo = $7;
	$max_sailor = $8;
	$max_cannon = $9;
	$min_sailor = $10;
	$lateen_sail = $11;
	$square_sail = $12;
	$wave_resistance = $13;
	$turning = $14;
	$base_armor = $15;
	$max_durability = $16;
	$desc = $17;

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
	print $RAWOUT "\t\t\"name\": \"$name\",\n";
	print $RAWOUT "\t\t\"size\": \"$size\",\n";
	print $RAWOUT "\t\t\"price\": $price,\n";
	print $RAWOUT "\t\t\"specialization\": \"$specialization\",\n";
	print $RAWOUT "\t\t\"max storage\": $max_storage,\n";
	print $RAWOUT "\t\t\"max cargo\": $max_cargo,\n";	
	print $RAWOUT "\t\t\"max cannons\": $max_cannon,\n";
	print $RAWOUT "\t\t\"max sailors\": $max_sailor,\n";
	print $RAWOUT "\t\t\"min sailors\": $min_sailor,\n";
	print $RAWOUT "\t\t\"lateen sail\": $lateen_sail,\n";
	print $RAWOUT "\t\t\"square sail\": $square_sail,\n";
	print $RAWOUT "\t\t\"wave resistance\": $wave_resistance,\n";
	print $RAWOUT "\t\t\"turning\": $turning,\n";
	print $RAWOUT "\t\t\"base armor\": $base_armor,\n";
	print $RAWOUT "\t\t\"max durability\": $max_durability";

	unless ($desc eq '')
		{print $RAWOUT ",\n\t\t\"desc\": \"$desc\"";}
	print $RAWOUT "\n\t}";
}

print $RAWOUT "\n]"
