/*Light master, Handles big boy tasks like assignning light values for rendered elements and lightning
Theres 4 major light levels:
The first is the background level, which covers the background and ultra weather only
The second extends right under player plane, covering background object and middle weather and anon ents under player bullets
The third extends from player bullets to player, covering all important gameplay elements
The last extends from particles to closest weather layer
These layers are enumerated 0 to 3, 0 being furthest and first rendered
*/
#include <mutex>

using namespace std;

class LightMaster {
	mutex lightLock;
};