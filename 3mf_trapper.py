#!/bin/python3
import sys
import argparse
import os
from pathlib import Path
import zipfile
import random
import string
from colorama import just_fix_windows_console, Style, Fore
import io

def unpack_model(archive, folder):
    with zipfile.ZipFile(archive, 'r') as zip_ref:
        zip_ref.extractall(folder)

def NiceClose(Level=0):
    print(Style.RESET_ALL)
    sys.exit(Level)

def FancyPrint(Text, color=Fore.YELLOW):
    print(color, '[o] \t' + Text)

def Gen3mf(payload):
    dirname = ''.join(random.choices(string.ascii_letters + string.digits, k=15 ))
    
    FancyPrint("Building directory structure and files")

    threed_dir = dirname + '/3D/'
    rels_dir = dirname + '/_rels/'
    
    os.makedirs(threed_dir, exist_ok=True)
    os.makedirs(rels_dir, exist_ok=True)

    write3d = open(threed_dir + '3dmodel.model', "w")
    write3d.write(payload)
    write3d.close()
    FancyPrint("The light is green, the TRAP is clean", Fore.GREEN)

    writerel = open(rels_dir + '.rels', "w")
    writerel.write('')
    writerel.close()

    MF_DIRS = []

    MF_DIRS.append(threed_dir)
    MF_DIRS.append(rels_dir)

    FancyPrint("Wrapping up our chimichanga! (creating zip file)")

    with zipfile.ZipFile('chimichanga.3mf', 'x', compresslevel=None, strict_timestamps=False) as zipMe:
        zipIt(MF_DIRS, zipMe)

    FancyPrint("All set! chimichanga.3mf ready for consumption.")

def FileRead(file):
    with open(file, "rb") as f:
        int_Arr = []
        agg_str = ''

        FancyPrint("Setting trap in 3dmodel")

        agg_str += '\n\t\t\t' + '<boot>' + '\n'

        FancyPrint("Let's convert some bytes and build some coordinates")

        while(byte := f.read(1)):
            dec_byte = int.from_bytes(byte, "big")
            int_Arr.append(dec_byte)
            if len(int_Arr) == 3:
                agg_str += "\t\t\t\t" + '<vertex x="' + str(int_Arr[0]) + '" y="' + str(int_Arr[1]) + '" z="' + str(int_Arr[2]) + '" />' + '\n'
                agg_str += "\t\t\t\t" + '<vertex x="-' + str(int_Arr[0]) + '" y="-' + str(int_Arr[1]) + '" z="-' + str(int_Arr[2]) + '" />' + '\n'
                int_Arr= []

        if ((len(int_Arr) < 3) and (len(int_Arr) > 0)):
            int_Arr.append(0 * (3 - len(int_Arr)))
            agg_str += "\t\t\t\t" + '<vertex x="' + str(int_Arr[0]) + '" y="' + str(int_Arr[1]) + '" z="' + str(int_Arr[2]) + '" />' + '\n'
            agg_str += "\t\t\t\t" + '<vertex x="-' + str(int_Arr[0]) + '" y="-' + str(int_Arr[1]) + '" z="-' + str(int_Arr[2]) + '" />' + '\n'

        agg_str += '\t\t\t' + '</boot>' + '\n'

    FancyPrint("Coordinate block complete")

    return agg_str

def zipIt(paths, zipHandle):
    for path in paths:
        for root,dirs,files in os.walk(path):
            for file in files:
                zipHandle.write(os.path.join(root,file), os.path.relpath(os.path.join(root,file),os.path.join(path, '..')))

def threedeeInject(archive, payload):
    with zipfile.ZipFile(archive, 'r') as Source_Zip:
        with io.BytesIO() as New_Buffer:
            with zipfile.ZipFile(New_Buffer, 'w') as New_Zip:
                for file in Source_Zip.infolist():
                    File_Content = Source_Zip.read(file)
        
                    if file.filename == '3D/3dmodel.model':
                        model_xml = File_Content.decode('utf-8')

                        trap_location = model_xml.find('</vertices>')

                        if trap_location != -1:
                            trapped_xml = model_xml[:trap_location + len('</vertices>')] + payload + model_xml[trap_location + len('</vertices>'):]
                            File_Content = (trapped_xml.encode('utf-8'))
                            FancyPrint("The light is green, the TRAP is clean", Fore.GREEN)
                        else:
                            FancyPrint("Could not find 'Vertices' section",Fore.RED)
                    
                    New_Zip.writestr(file,File_Content)

            Source_Zip.close

            FancyPrint("Overwriting 3mf with trapped content")

            try:
                with open(archive, 'wb') as TrappedZip:
                    TrappedZip.write(New_Buffer.getvalue())
            except IOError:
                FancyPrint("Error writing to file, make sure it isn't opened already", Fore.RED)
            


def main(options):

    inputfile= ""

    if options.InputFile:
        inputfile = options.InputFile

        if (os.path.exists(inputfile) & os.path.isfile(inputfile)):
            bin23ml = FileRead(inputfile)
            if options.Model:
                if (os.path.exists(options.Model) & os.path.isfile(options.Model)):
                    threedeeInject(options.Model, bin23ml)
                else:
                    FancyPrint("Either the 3mf file is not a file or it does not exist!!", Fore.RED)
                    NiceClose()
            else:
                Gen3mf(bin23ml)
        else:
            FancyPrint("Either the input file is not a file or it does not exist!!", Fore.RED)
            NiceClose()
    NiceClose()
    


if __name__ == "__main__":
    just_fix_windows_console()
    parser = argparse.ArgumentParser(description='A nice little helper script to read a file as bytes and create a block of coordinates out of them')

    parser.add_argument('-i', '--InputFile', type=Path, help='File to Insert', required=True)
    parser.add_argument('-m', '--Model', type=Path, help='Load in 3mf')

    args = parser.parse_args()
    
    main(args)

