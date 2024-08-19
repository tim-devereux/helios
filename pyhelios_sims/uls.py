import pyhelios
from pyhelios.util import scene_writer
import time
import os
import subprocess
from datetime import datetime
import pandas as pd
os.chdir("..")

# woody_mesh_file = "data/sceneparts/litch21_uls/litch21_woody_mesh.obj"
# leaf_mesh_file = "data/sceneparts/litch21_uls/litch21_leaf_mesh.obj"
# ground_mesh_file = "data/sceneparts/litch21_uls/litch21_ground_mesh.obj"
# voxel_file = "data/sceneparts/litch21_uls/litch21_us.xyz"

# scene_file = "data/scenes/litch21_uls/litch21_uls.xml"
# survey_file = "data/surveys/litch21_uls/litch21_uls.xml"

# sp_list = []

# print("Creating woody_mesh scene part...")
# woody_mesh = scene_writer.create_scenepart_obj(woody_mesh_file)
# print("Mesh scene part created.")

# print("Creating leaf mesh scene part...")
# leaf_mesh = scene_writer.create_scenepart_obj(leaf_mesh_file)
# print("Leaf mesh scene part created.")

# print("Creating ground mesh scene part...")
# ground_mesh = scene_writer.create_scenepart_obj(ground_mesh_file)
# print("Ground mesh scene part created.")

# print("Creating voxels scene part...")
# voxels = scene_writer.create_scenepart_xyz(voxel_file, voxel_size = 0.05)
# print("Voxels scene part created.")

# sp_list.append(woody_mesh)
# sp_list.append(leaf_mesh)
# sp_list.append(ground_mesh)
# sp_list.append(voxels)

# print("Building the scene...")
# scene = scene_writer.build_scene(scene_id="litch21_uls", name="litch21_uls", sceneparts=sp_list)
# print("Scene built.")

# print(f"Writing scene to {scene_file}...")
# with open(scene_file, "w") as f:
#     f.write(scene)
# print(f"Scene written to {scene_file}.")

# # Build simulation parameters
# print("Building simulation parameters...")
# simBuilder = pyhelios.SimulationBuilder(
#         survey_file,
#         'assets/',
#         'output/',
#     )

# simBuilder.setLasOutput(True)
# simBuilder.setLas10(True)
# simBuilder.setNumThreads(6)

# print("Building the simulation...")
# sim = simBuilder.build()
# print("Simulation built.")

# print("done building")

# # Start the simulation.
# print("Starting the simulation...")
# start_time = time.time()
# sim.start()

# if sim.isStarted():
#     print('Simulation has started!\nSurvey Name: {survey_name}\n{scanner_info}'.format(
#         survey_name = sim.sim.getSurvey().name,
#         scanner_info = sim.sim.getScanner().toString()))

# while sim.isRunning():
#     duration = time.time()-start_time
#     mins = duration // 60
#     secs = duration % 60
#     print("\r"+"Simulation running for {} min and {} sec.".format(int(mins), int(secs)), end="")
#     time.sleep(1)

# # Create instance of PyHeliosOutputWrapper class using sim.join(). 
# # Contains attributes 'measurements' and 'trajectories' which are Python wrappers of classes that contain the output vectors.
# print("Joining simulation results...")
# output = sim.join()
# print("Simulation results joined.")

# # Create instances of vector classes by accessing 'measurements' and 'trajectories' attributes of output wrapper.
# measurements = output.measurements
# trajectories = output.trajectories

# # Get amount of points in trajectory and amount of measurements by accessing length of measurement and trajectory vectors.
# print('\nNumber of measurements : {n}'.format(n=len(measurements)))
# print('Number of points in trajectory : {n}'.format(n=len(trajectories)))

# print("\n"+"All simulations have finished!")


# Base directory where the folders are created
out_dir = "output/litch21_uls"

# Get the list of directories in the base directory
dirs = [os.path.join(out_dir, d) for d in os.listdir(out_dir) if os.path.isdir(os.path.join(out_dir, d))]

# Sort the directories by creation time (most recent first)
dirs.sort(key=os.path.getctime, reverse=True)

# Get the most recent directory
latest_dir = dirs[0] if dirs else None

if latest_dir:
    # Get all .las files and corresponding .txt files in the latest directory
    las_files = [f for f in os.listdir(latest_dir) if f.endswith("_points.las")]
    txt_files = [f for f in os.listdir(latest_dir) if f.endswith("_trajectory.txt")]

    # Sort to match files by leg number
    las_files.sort()
    txt_files.sort()

    # Preprocess each .txt file (move the 4th column to the front)
    for txt_file in txt_files:
        traj_path = os.path.join(latest_dir, txt_file)
        
        # Check if the file is empty
        if os.stat(traj_path).st_size == 0:
            print(f"Skipping empty file: {traj_path}")
            continue
        
        try:
            df = pd.read_csv(traj_path, sep='\s+', header=None)
            # Move the 4th column to the front
            df = df[[3, 0, 1, 2] + list(range(4, df.shape[1]))]
            # Save the modified file back to disk
            df.to_csv(traj_path, sep=' ', header=False, index=False)
        except pd.errors.EmptyDataError:
            print(f"Skipping file with parsing issues: {traj_path}")
            continue

    # Run the command for each pair
    for las_file, txt_file in zip(las_files, txt_files):
        cloud = os.path.join(latest_dir, las_file)
        traj = os.path.join(latest_dir, txt_file)

        # Execute the command
        subprocess.run(f"apptainer exec --bind /home/tim/Code/helios/{latest_dir}:/data /home/tim/containers/raycloudtools_private_latest.sif rayimport /data/{las_file} /data/{txt_file}", shell=True)
else:
    print("No directories found in the specified base directory.")