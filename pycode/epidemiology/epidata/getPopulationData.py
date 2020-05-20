import os
import sys
import pandas
from epidemiology.epidata  import getDataIntoPandasDataFrame as gd
from epidemiology.epidata  import outputDict as od
from epidemiology.epidata import defaultDict as dd


def get_population_data(read_data=dd.defaultDict['read_data'],
                                make_plot=dd.defaultDict['make_plot'],
                                out_form=dd.defaultDict['out_form'],
                                out_folder=dd.defaultDict['out_folder']):

    file1 = os.path.join(out_folder, "FullDataB.json")
    file2 = os.path.join(out_folder, "FullDataL.json")      

    if(read_data):
        # if once dowloaded just read json file
        
        try:
            dfB = pandas.read_json(file1)

        except ValueError:
            exit_string = "Error: The file: " + file1 + "does not exist. Call program without -r flag to get it."
            sys.exit(exit_string)
            
        try:
            dfL = pandas.read_json(file2)

        except ValueError:
            exit_string = "Error: The file: " + file2 + "does not exist. Call program without -r flag to get it."
            sys.exit(exit_string)

    else:

        # Supported data formats:
        load = { 
            'csv': gd.loadCsv,
            'geojson': gd.loadGeojson
         }

        # ArcGIS public data item ID:
        itemIdB = '5dc2fc92850241c3be3d704aa0945d9c_2'
        itemIdL = 'b2e6d8854d9744ca88144d30bef06a76_1'

        # Get data:
        dfB = load['csv'](itemIdB)
        dfL = load['csv'](itemIdL)

        # output data to not always download it
        dfB.to_json(file1)
        dfL.to_json(file2)

    
    # Preperation for plotting/output:

    outForm = od.outForm[out_form][0]
    outFormEnd = od.outForm[out_form][1]
    outFormSpec = od.outForm[out_form][2]

    print("Available columns for states:", dfB.columns)
    print("Available columns for counties:", dfL.columns)  

     # Filter data for Bundesland/Landkreis and Einwohnerzahl (EWZ)

    dfB = dfB.rename(columns={'LAN_ew_GEN': 'Bundesland', 'LAN_ew_EWZ': 'EWZ'})
    dfBo = dfB[['Bundesland','EWZ']]
    getattr(dfBo, outForm)(os.path.join(out_folder,  "PopulStates" + outFormEnd), **outFormSpec)
  

    # TODO Counties are less as in RKI data. Compare it!
    dfL = dfL.rename(columns={'GEN': 'Landkreis'})
    dfLo = dfL[['Landkreis','EWZ']]
    getattr(dfLo, outForm)(os.path.join(out_folder,  "PopulCounties" + outFormEnd), **outFormSpec)


def main():

    [read_data, make_plot, out_form, out_folder] = gd.cli('Download population data')
    get_population_data(read_data, make_plot, out_form, out_folder)


if __name__ == "__main__":

    main()
