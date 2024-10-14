import bpy
import json

# Set the object you want to export
obj = bpy.context.active_object
light = None

# Check if the object has children
if obj is not None and obj.children:
    # Loop through the children of the selected object
    for child in obj.children:
        if child.name == "Spot":  # Replace with the actual name of the child light object
            light = child
            print(f"Child light found: {light.name}")
            break
else:
    print("No children found for the selected object.")

# Get scene and frame range information
scene = bpy.context.scene
start_frame = scene.frame_start
end_frame = scene.frame_end

# Initialize a dictionary to hold animation data
animation_data = {
    "object_name": obj.name,
    "frames": []
}

# Iterate over each frame in the frame range
for frame in range(start_frame, end_frame + 1):
    scene.frame_set(frame)  # Set the current frame
    
    # Get world space transformation data for the object
    location = obj.matrix_world.to_translation()
    rotation = obj.matrix_world.to_euler()  # Euler angles (XYZ rotation)
    scale = obj.matrix_world.to_scale()
    
    # Store the transformation data for the current frame
    frame_data = {
        "frame": frame,
        "location": {
            "x": location.x,
            "y": location.y,
            "z": location.z
        },
        "rotation": {
            "x": rotation.x,
            "y": rotation.y,
            "z": rotation.z
        },
        "light": {
            "power": light.data.energy,
            "angle": light.rotation_euler.x 
        }
        
        
    }
    
    animation_data["frames"].append(frame_data)

# Save the animation data as a JSON file
output_file = "/Users/julesbernard/Documents/Github/ddd-opti/3D_stage/path.01/animation.json"  # Set your file path here
with open(output_file, 'w') as f:
    json.dump(animation_data, f, indent=4)

print(f"Animation data saved to {output_file}")
        