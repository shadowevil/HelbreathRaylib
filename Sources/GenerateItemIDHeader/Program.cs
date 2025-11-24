using System.Text;
using System.Text.Json;

string FindProjectRoot(string startPath)
{
    var dir = new DirectoryInfo(startPath);
    while (dir != null)
    {
        if (File.Exists(Path.Combine(dir.FullName, "Binaries", "Game", "DATA", "ItemMetadata.json")))
        {
            return dir.FullName;
        }
        dir = dir.Parent;
    }
    throw new DirectoryNotFoundException("Project root not found");
}

string solutionDir = args.Length > 0 ? args[0].TrimEnd('\\', '"') : FindProjectRoot(AppContext.BaseDirectory);

string inputPath = Path.Combine(solutionDir, "Binaries", "Game", "DATA", "ItemMetadata.json");
string outputPath = Path.Combine(solutionDir, "Sources", "Dependencies", "Shared", "ItemIDs.h");

if (!File.Exists(inputPath))
{
    Console.WriteLine($"Error: ItemMetadata.json not found at {inputPath}");
    return;
}

var json = File.ReadAllText(inputPath);
var items = JsonSerializer.Deserialize<JsonElement>(json);

var sb = new StringBuilder();
sb.AppendLine("#pragma once");
sb.AppendLine("#include <cstdint>");
sb.AppendLine();
sb.AppendLine("namespace ItemID {");
sb.AppendLine("\tenum : int32_t {");
sb.AppendLine("\t\tNONE = -1,");

int index = 0;
foreach (var item in items.EnumerateArray())
{
    string pakFile = item.GetProperty("pak_file").GetString();
    int itemId = item.GetProperty("id").GetInt32();
    string name = pakFile.Replace(".pak", "")
                         .Replace("-", "_")
                         .Replace(" ", "_")
                         .ToUpper();

    name = new string(Array.FindAll(name.ToCharArray(),
        c => char.IsLetterOrDigit(c) || c == '_'));

    sb.AppendLine($"\t\t{name} = {itemId},");
    index++;
}

sb.AppendLine("\t};");
sb.AppendLine("}");

File.WriteAllText(outputPath, sb.ToString());
Console.WriteLine($"Generated ItemIDs.h with {index} items");