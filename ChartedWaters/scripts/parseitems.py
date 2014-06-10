#!/usr/bin/env python

import json
import re

#files = ["food", "rawwaterials", "other", "luxury"]
files = ["food"]
regex = re.compile(r"([^\t]*)\t?") # Match a sequence of non-tabs optionally followed by a tab.

for filename in files:
    with open("protojson/" + filename + ".protojson", "r") as file:      
        output = []
        for line in file:
            buffer = dict()
            properties = regex.findall(line)
            counter = 0
            buffer["category"] = properties[counter]; counter += 1
            buffer["type"] = properties[counter]; counter += 1
            buffer["name"] = properties[counter]; counter += 1
            buffer["ID"] = buffer["name"];
            buffer["base price"] = int(properties[counter]); counter += 1
            if (properties[counter] != ""):
                buffer["desc"] = properties[counter]
            counter += 1
            buffer["method"] = properties[counter]; counter += 1
            buffer["specifics"] = properties[counter]; counter += 1
            if (properties[counter] != ""):
                buffer["multiplier"] = int(properties[counter])
            counter += 1
            fname = properties[counter]
            if (fname[-1] == "\n"):
                buffer["facility name"] = fname[:-1]
            else:
                buffer["facility name"] = fname
            
            output.append(buffer)

        jsonfile = open("out/" + filename + ".json", "w")
        jsonfile.write(json.dumps(output, sort_keys=True, indent=4, separators=(',',': ')))