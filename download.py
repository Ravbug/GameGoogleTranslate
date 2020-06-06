# Requires python 3

import argparse
parser = argparse.ArgumentParser(description='Download the translation spreadsheet and write it as a TSV file')
parser.add_argument('--id', type=str,help='The ID of the spreadsheet')
parser.add_argument('--format',type=str,help="The format to export as",default="tsv")
args = parser.parse_args()

# download the spreadsheet from google as a TSV
import requests
print("Downloading file")
data = requests.get("https://docs.google.com/spreadsheets/d/" + args.id + "/export?gid=0&format="+args.format).text

print("Writing translated."+args.format)
with open("translated." + args.format,"w") as f:
    f.write(data)