# SCRIPT TO GZIP CRITICAL FILES FOR ACCELERATED WEBSERVING
# see also https://community.platformio.org/t/question-esp32-compress-files-in-data-to-gzip-before-upload-possible-to-spiffs/6274/10
#
# Ampliamente modificado  Ver original en el link citado anteriormente
Import( 'env', 'projenv' )
import os
import shutil
import glob
import json
from re import sub
# Comprime los archivos definidos de 'data_src/' en 'data/'
def renameReactAssets( source, target, env ):
    # Build the react web application
    react_proj_dir = "react-websocket-client"
    os.chdir(react_proj_dir)
    print( '\nReact build: Starting the build process for the React webapp...\n' )
    
    print('\nRunning `npm install`')
    if os.system("npm install") != 0:
        print(f"Error installing dependencies for react application in ./{react_proj_dir}")
        return
    print('\nThe 6 high severity vulnerabilities above is known issue of react-scripts, no extra action is needed')
    print('\nRunning `npm run build`')
    if os.system("npm run build") != 0:
        print(f"Error building react application in ./{react_proj_dir}")
        return    
    os.chdir('..')    
    print( '\nRename: Starting the renaming process for the SPIFFS image...\n' )
    data_src_dir_path = os.path.join(env.get('PROJECT_DIR'), f"{react_proj_dir}/build")
    data_dir_path = os.path.join(env.get('PROJECT_DIR'), 'data')
    # delete existing folder
    if(os.path.exists(data_dir_path)):
        print('Rename: Deleting the old "data" directory ' + data_dir_path)
        shutil.rmtree(data_dir_path)
    print('Rename: Re-creating an data directory ' + data_dir_path)
    os.mkdir(data_dir_path)
    shutil.move(data_src_dir_path, data_dir_path + "/www")
env.AddPreAction( '$BUILD_DIR/littlefs.bin', renameReactAssets )
