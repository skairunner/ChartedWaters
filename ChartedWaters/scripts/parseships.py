#!/usr/bin/env python

import json
import re

output = []

file = open("protojson/ships.protojson", "r")

regex = re.compile(r"([^\t]*)\t?") # Match a sequence of non-tabs optionally followed by a tab.


# A class for inserting properties that may be variable.
def insertElement(out, key, string, datatype = "string"):
    strs = string.split(",")
    for x in range(0, len(strs)):
        strs[x] = convert(string, datatype)
    out[key] = strs            

for line in file:
    buffer = dict()
    properties = regex.findall(line)
    counter = 0
    buffer["ID"] = properties[counter]; counter += 1
    buffer["name"] = properties[counter]; counter += 1
    buffer["size"] = properties[counter]; counter += 1
    buffer["price"] = int(properties[counter]); counter += 1
    buffer["specialization"] = properties[counter]; counter += 1
    buffer["max storage"] = int(properties[counter]); counter += 1
    buffer["max cargo"] = int(properties[counter]); counter += 1
    buffer["max sailors"] = int(properties[counter]); counter += 1
    buffer["max cannons"] = int(properties[counter]); counter += 1
    buffer["min sailors"] = int(properties[counter]); counter += 1
    buffer["lateen"] = int(properties[counter]); counter += 1
    buffer["square"] = int(properties[counter]); counter += 1
    buffer["wave resistance"] = int(properties[counter]); counter += 1
    buffer["turning"] = int(properties[counter]); counter += 1
    buffer["base armor"] = int(properties[counter]); counter += 1
    buffer["max durability"] = int(properties[counter]); counter += 1
    buffer["sail slots"] = int(properties[counter]); counter += 1
    buffer["cannon slots"] = int(properties[counter]); counter += 1
    buffer["fore cannon slots"] = int(properties[counter]); counter += 1
    buffer["aft cannon slots"] = int(properties[counter]); counter += 1
    buffer["armor slots"] = int(properties[counter]); counter += 1
    buffer["desc"] = properties[counter]; counter += 1

    output.append(buffer)

jsonfile = open("out/ships.json", "w")
jsonfile.write(json.dumps(output, sort_keys=True, indent=4, separators=(',',': ')))