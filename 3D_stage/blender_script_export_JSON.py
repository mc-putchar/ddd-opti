import bpy
import json
import math

# Specify the path for the output file
output_file_path = "/Users/julesbernard/Documents/Github/ddd-opti/3D_stage/path.01/animationB01.json"


def radians_to_degrees(radians):
    return radians * (180.0 / math.pi)
	
# Function to export the action data to JSON
def export_action_to_json(object_name, filepath):
    obj = bpy.data.objects[object_name]

    # Ensure the object has an action
    if not obj.animation_data or not obj.animation_data.action:
        print(f"No action found for object: {object_name}")
        return

    action = obj.animation_data.action
    start_frame = int(action.frame_range[0])
    end_frame = int(action.frame_range[1])

    frames = []

    # Find the first light among the children
    light = None
    for child in obj.children:
        if child.type == 'LIGHT':
            light = child
            break

    # Loop through each frame in the action
    for frame in range(start_frame, end_frame + 1):
        bpy.context.scene.frame_set(frame)  # Set the current frame
        
        # Capture object's location and rotation
        location = obj.location.copy()
        rotation = obj.rotation_euler.copy()
        
        # Prepare the frame data
        frame_data = {
            "frame": frame,
            "location": {
                "x": location.x,
                "y": location.y,
                "z": location.z
            },
            "rotation": {
                "z": radians_to_degrees(rotation.z)
            },
            "light": {
                "power": 0.0,  # Default values for light
                "angle": 0.0
            }
        }

        # If a light is found, capture its properties
        if light:
            # Set the light's properties
            light_location = light.location.copy()
            light_rotation = light.rotation_euler.copy()
            light_power = light.data.energy
            
            # Set light data in frame data
            frame_data["light"]["power"] = light_power
            frame_data["light"]["angle"] = radians_to_degrees(light_rotation.x)

        # Add the frame data to the list
        frames.append(frame_data)

    # Create a dictionary for the export
    export_data = {
        "object_name": object_name,
        "frames": frames
    }

    # Write to JSON file
    with open(filepath, 'w') as json_file:
        json.dump(export_data, json_file, indent=4)

    print(f"Exported action data to {filepath}")

# Example usage
export_action_to_json("Drone.001", output_file_path)  # Change this path accordingly


