import unreal
from unreal import EditorAssetLibrary
from unreal import Paths
from unreal import RenderingLibrary
from unreal import EditorLevelLibrary

unreal.log("hello from python!")

unreal.log(EditorAssetLibrary.does_directory_exist("/Game/Membrane20"))

membrane_dir = "/Game/NiagaraSystems/Membrane20"
membrane_data_dir = membrane_dir + "/Data"


#
# Make the directories needed to store the data files
#
def make_data_directories():
    numStages = 6

    for i in range(numStages):
        dir = membrane_data_dir + "/ST" + str(i + 1) + "_Data"
        if EditorAssetLibrary.does_directory_exist(dir):
            continue

        if not EditorAssetLibrary.make_directory(dir):
            unreal.log("Could not make directory: " + dir)
        else:
            unreal.log("Directory Created: " + dir)


#
# Gets the template Data Table file in order to create the appropriate ones
#
def get_dt_template():
    return EditorAssetLibrary.load_asset(membrane_data_dir + "/DT_Template")

def get_crt_template():
    return EditorAssetLibrary.load_asset(membrane_data_dir + "/CRT_Template")
##
#
def stage_folder_path(stageNum):
    content_dir = Paths.project_content_dir()
    dir = "NiagaraSystems/Membrane20/Data/"
    folder = "ST" + str(stageNum) + "_Data"
    return ("/Game/" + dir + folder , content_dir + dir + folder )

def stage_raw_data_path(stageNum):
    contentDir = Paths.project_content_dir()
    data_dir = contentDir + "NiagaraSystems/Membrane20/Data/"
    return data_dir + "ST" + str(stageNum) + "_Data/RawData/"
#
# Creates the Data Table file to dst, using the csv file at csv_path and the dt template
#
def create_dt_from_csv(asset_name, dst_path, csv_path, dt_template):
    if not dt_template:
        return False

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

    unreal.log(asset_name + " " + dst_path + " " + csv_path)
    new_dt = asset_tools.duplicate_asset(asset_name, dst_path, dt_template)
    unreal.DataTableFunctionLibrary.fill_data_table_from_csv_file(new_dt, csv_path)


#
# Create / Write Data Tables for each Stage
#
def write_data_tables_per_stage():
    dt_templ = get_dt_template()

    numStages = 6
    file_names = ["Pos_Clathrin", "Pos_Membrane", "Pos_MemAdapter", "Pos_Substrate"]

    for i in range(numStages):
        for j in range(len(file_names)):
            cur_fname = file_names[j]
            dst_game, dst_disk = stage_folder_path(i + 1)
            create_dt_from_csv("DT_" + cur_fname, dst_game, dst_disk + "/RawData/" + cur_fname + ".csv", dt_templ)

#
# Create / Write Canvas Render Target for each Stage 
#

# gets the asset specified by its name and path.
# if it doesn't exist, it will create it
# returns the asset
def get_asset(asset_name, dst_path, create = False, asset_templ = None):
    if EditorAssetLibrary.does_asset_exist(dst_path + "/" + asset_name):
        return EditorAssetLibrary.load_asset(dst_path + "/" + asset_name)
    elif create:
        unreal.log("Asset Created: " + dst_path + "/" + asset_name)
        asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
        return asset_tools.duplicate_asset(asset_name, dst_path, asset_templ)

    return None

def write_canvas_render_targets_per_stage():
    crt_templ = get_crt_template()

    numStages = 6
    file_names = ["Pos_Clathrin", "Pos_Membrane", "Pos_MemAdapter", "Pos_Substrate"]

    for stageNum in range(numStages):
        for j in range(len(file_names)):
            cur_fname = file_names[j]
            dst_game, dst_disk = stage_folder_path(stageNum + 1)

            dt = get_asset("DT_" + cur_fname, dst_game)
            if not dt:
                unreal.log("DT not found! Name: " + dst_game + "/DT_" + cur_fname)
                continue

            crt = get_asset("CRT_" + cur_fname, dst_game, create=True, asset_templ=crt_templ)
            if not crt:
                unreal.log("crt not found!")
                continue

            numRows = len(unreal.DataTableFunctionLibrary.get_data_table_row_names(dt))

            dt_X = unreal.DataTableFunctionLibrary.get_data_table_column_as_string(dt, "XPos")
            dt_Y = unreal.DataTableFunctionLibrary.get_data_table_column_as_string(dt, "YPos")
            dt_Z = unreal.DataTableFunctionLibrary.get_data_table_column_as_string(dt, "ZPos")

            crt.set_editor_property("size_x", 1)
            crt.set_editor_property("size_y", numRows)

            # we need a world context object (an object whose GetWorld() is valid) 
            # just get it from the editor level (shouldn't be empty!)
            actors = EditorLevelLibrary.get_all_level_actors()
            canvas, size, context = RenderingLibrary.begin_draw_canvas_to_render_target(actors[0], crt)

            # todo: test out squashing data via Sigmoid and inversing it later on so that we can make the RT store float8 and save it statically... (not needed if we intend to rebuild the RT's every time)

            for i in range(len(dt_X)):
                spos = unreal.Vector2D(0, 0)
                ssize = unreal.Vector2D(1, 1)
                lcolor = unreal.LinearColor(float(dt_X[i]), float(dt_Y[i]), float(dt_Z[i]), 1)

                canvas.draw_texture(None, unreal.Vector2D(0, i), ssize, spos, ssize, lcolor)

            RenderingLibrary.end_draw_canvas_to_render_target(actors[0], context)

write_canvas_render_targets_per_stage()


def TempFacesProcedure():
    dst_game, dst_disk = stage_folder_path(6)

    crt_templ = get_crt_template()
    dt = get_asset("Faces_Membrane", dst_game)
    crt = get_asset("CRT_Faces_Membrane", dst_game, create=True, asset_templ=crt_templ)

    numRows = len(unreal.DataTableFunctionLibrary.get_data_table_row_names(dt))

    dt_X = unreal.DataTableFunctionLibrary.get_data_table_column_as_string(dt, "XPos")
    dt_Y = unreal.DataTableFunctionLibrary.get_data_table_column_as_string(dt, "YPos")
    dt_Z = unreal.DataTableFunctionLibrary.get_data_table_column_as_string(dt, "ZPos")

    crt.set_editor_property("size_x", 1)
    crt.set_editor_property("size_y", numRows)

    # we need a world context object (an object whose GetWorld() is valid) 
    # just get it from the editor level (shouldn't be empty!)
    actors = EditorLevelLibrary.get_all_level_actors()
    canvas, size, context = RenderingLibrary.begin_draw_canvas_to_render_target(actors[0], crt)

    # todo: test out squashing data via Sigmoid and inversing it later on so that we can make the RT store float8 and save it statically... (not needed if we intend to rebuild the RT's every time)

    for i in range(len(dt_X)):
        spos = unreal.Vector2D(0, 0)
        ssize = unreal.Vector2D(1, 1)
        lcolor = unreal.LinearColor(float(dt_X[i]), float(dt_Y[i]), float(dt_Z[i]), 1)

        canvas.draw_texture(None, unreal.Vector2D(0, i), ssize, spos, ssize, lcolor)

    RenderingLibrary.end_draw_canvas_to_render_target(actors[0], context)

TempFacesProcedure()
"""
new_crt = get_asset("test_crt", "/Game/Membrane20/Data", create=True, asset_templ=crt_templ)
    
test_dt = get_asset("DT_Pos_Membrane", stage_folder_path(2)[0])

if new_crt and test_dt:
    numRows = len(unreal.DataTableFunctionLibrary.get_data_table_row_names(test_dt))

    dt_X = unreal.DataTableFunctionLibrary.get_data_table_column_as_string(test_dt, "XPos")
    dt_Y = unreal.DataTableFunctionLibrary.get_data_table_column_as_string(test_dt, "YPos")
    dt_Z = unreal.DataTableFunctionLibrary.get_data_table_column_as_string(test_dt, "ZPos")

    unreal.log(dt_X)
    
    new_crt.set_editor_property("size_x", 1)
    new_crt.set_editor_property("size_y", numRows)

    # we need a world context object (an object whose GetWorld() is valid) 
    # just get it from the editor level (shouldn't be empty!)
    actors = EditorLevelLibrary.get_all_level_actors()
    canvas, size, context = RenderingLibrary.begin_draw_canvas_to_render_target(actors[0], new_crt)
    
    # todo: test out squashing data via Sigmoid and inversing it later on so that we can make the RT store float8 and save it statically... (not needed if we intend to rebuild the RT's every time)
    
    for i in range(len(dt_X)):
        spos = unreal.Vector2D(0, 0)
        ssize = unreal.Vector2D(1, 1)
        lcolor = unreal.LinearColor(float(dt_X[i]), float(dt_Y[i]), float(dt_Z[i]), 1)

        canvas.draw_texture(None, unreal.Vector2D(0, i), ssize, spos, ssize, lcolor)

    RenderingLibrary.end_draw_canvas_to_render_target(actors[0], context)

"""