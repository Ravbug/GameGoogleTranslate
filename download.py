# Requires python 3

import argparse
parser = argparse.ArgumentParser(description='Download the translation spreadsheet and write it as a TSV file')
parser.add_argument('--id', type=str,help='The ID of the spreadsheet')
args = parser.parse_args()


# download the spreadsheet from google as a TSV
import requests
sheetid = args.id
print("Downloading file")
data = requests.get("https://docs.google.com/spreadsheets/d/"+ sheetid +"/gviz/tq?tq=SELECT%20C").text

# parse the JSON
print("Parsing JSON")
import json
table = json.loads(data[data.index('{'):-2])

# combine it into an array
tsv = []
for cel in table["table"]["rows"]:
    tsv.append("\"" + cel['c'][0]['v'] + "\"")

# write to file
print("Writing translated.tsv")
with open("translated.tsv","w") as f:
    f.write("\r\n".join(tsv))