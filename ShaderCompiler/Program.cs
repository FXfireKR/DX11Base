using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.Diagnostics;

// args[0] : Compile Shader Path
// args[1] : Compile Shader OutPath
// args[2] : Shader json

try
{
    // check argument size
    if (args.Length < 3) {
        throw new ArgumentException("argument not enough : " + args.Length);
    }

    string strShaderPath = args[0];
    string strOutputPath = args[1];
    string strShaderJson = args[2];

    if (false == File.Exists(strShaderJson)) {
        throw new FileNotFoundException(strShaderJson + " not found file");
    }

    string shadersData = File.ReadAllText(strShaderJson);
    JObject jObj = JObject.Parse(shadersData);

    foreach (var prop in jObj) {
        //prop.Key
        if (null == prop.Value) continue;

        foreach(var shader in prop.Value) {
            JProperty? jProperty = shader as JProperty;
            if (null == jProperty) continue;

            string? shaderPath = "";
            string? shaderEntry = "";
            string? shaderVersion = "";

            foreach (var elem in shader) {
                shaderPath = elem.Value<string>("path");
                shaderEntry = elem.Value<string>("entry");
                shaderVersion = elem.Value<string>("version");
            }

            if (string.IsNullOrEmpty(shaderPath) ||
                string.IsNullOrEmpty(shaderEntry) ||
                string.IsNullOrEmpty(shaderVersion)) {
                Console.WriteLine(prop.Key + " path, entry, version is null or empty!");
                continue;
            }


            string shaderFile = Path.Combine(strShaderPath, shaderPath);
            if (false == File.Exists(shaderFile + ".hlsl")) {
                Console.WriteLine(shaderFile + " doesn't exist!");
                continue;
            }

            Process proc = new Process();
            proc.StartInfo.FileName = "fxc.exe";
            proc.StartInfo.ArgumentList.Add("/T");
            proc.StartInfo.ArgumentList.Add(shaderVersion);
            proc.StartInfo.ArgumentList.Add("/E");
            proc.StartInfo.ArgumentList.Add(shaderEntry);

            // 일단은 define이 없다고 가정
            //proc.StartInfo.ArgumentList.Add("/D");

            proc.StartInfo.ArgumentList.Add("/Fo");
            proc.StartInfo.ArgumentList.Add(shaderFile + jProperty.Name + ".cso");
            proc.StartInfo.ArgumentList.Add(shaderFile + ".hlsl");
            proc.Start();
        }
    }

}
catch(Exception e)
{
    Console.WriteLine(e.Message);
    return;
}