#include "PointCloud.hpp"
#include "Viewer.hpp"

int
usage(int argc, char * argv[])
{
  DGP_CONSOLE << "";
  DGP_CONSOLE << "Usage: " << argv[0] << " [OPTIONS] <points-in> [<points-out>]";
  DGP_CONSOLE << "";
  DGP_CONSOLE << "Options:";
  DGP_CONSOLE << "--normals     :   Estimate the normal at each point";
  DGP_CONSOLE << "--downsample  :   Adaptively downsample the points";
  DGP_CONSOLE << "";

  return -1;
}

int
main(int argc, char * argv[])
{
  if (argc < 2)
    return usage(argc, argv);

  bool normals = false;
  bool downsample = false;
  std::string in_path;
  std::string out_path;

  int pos_arg = 0;
  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];
    if (arg == "--normals")
      normals = true;
    else if (arg == "--downsample")
      downsample = true;
    else if (pos_arg == 0)
    {
      in_path = arg;
      pos_arg++;
    }
    else if (pos_arg == 1)
    {
      out_path = arg;
      pos_arg++;
    }
    else
      return usage(argc, argv);
  }

  if (pos_arg <= 0)
    return usage(argc, argv);

  PointCloud pcloud;
  if (!pcloud.load(in_path))
    return -1;

  DGP_CONSOLE << "Read " << pcloud.numPoints() << " points from " << in_path;

  if (normals)
    pcloud.estimateNormals();

  if (downsample)
    pcloud.adaptiveDownsample();

  if (!out_path.empty())
  {
    if (!pcloud.save(out_path))
      return -1;

    DGP_CONSOLE << "Saved point cloud to " << out_path;
  }

  Viewer viewer;
  viewer.setObject(&pcloud);
  viewer.launch(argc, argv);

  return 0;
}
