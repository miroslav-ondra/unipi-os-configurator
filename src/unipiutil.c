
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>


char UNIPI_ID_SYSFS[] = "/sys/devices/platform/unipi-id/unipi-id";
#define MAX_DATA 2048

char* get_unipi_id_item(const char* item, int trunc)
{
   int res;
   char *data, *lf;
   char fname[2048];

   snprintf(fname, 2047, "%s/%s", UNIPI_ID_SYSFS, item);
   fname[2047] = '\0';
   //printf("%s\n", fname);

   int f = open(fname, O_RDONLY);
   if (f < 0)
      return NULL;

   data = malloc(MAX_DATA);
   if (data == NULL) goto err;
   res = read(f, data, MAX_DATA-1);
   if (res <= 0) goto err1;
   data[res] = '\0';
   if (trunc) {
      lf = strchr(data,'\n');
      if (lf) *lf = '\0';
   }
   close(f);
   return data;

err1:
   free(data);
err:
   close(f);
   return NULL;
}

int for_each_module_id(int (*callback)(int, int, void*), void* cbdata)
{
    int slot, module_id;
    DIR *d;
    struct dirent *dir;
    char prefix[] = "module_id.";
    char* value;

    d = opendir(UNIPI_ID_SYSFS);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if ((dir->d_type == DT_REG)&&( strstr(dir->d_name, prefix) == dir->d_name)) {
                if (sscanf(dir->d_name+strlen(prefix), "%d", &slot) == 1) {
                    value = get_unipi_id_item(dir->d_name, 1);
                    if (value && (sscanf(value, "%x", &module_id)==1)) {
                        free(value);
                        if (callback(slot, module_id, cbdata) != 0) break;
                        //printf("%s %d %d\n", dir->d_name, slot, module_id);
                    } else if (value) free(value);
                }
            }
        }
        closedir(d);
    }
    return(0);
}

int for_each_module_description(int (*callback)(int, const char *, void*), void* cbdata)
{
    int slot;
    DIR *d;
    struct dirent *dir;
    char prefix[] = "module_description.";

    d = opendir(UNIPI_ID_SYSFS);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if ((dir->d_type == DT_REG)&&( strstr(dir->d_name, prefix) == dir->d_name)) {
                if (sscanf(dir->d_name+strlen(prefix), "%d", &slot) == 1) {
                    if (callback(slot, dir->d_name, cbdata) != 0) break;
                    //printf("%s %d\n", dir->d_name, slot);
                }
            }
        }
        closedir(d);
    }
    return(0);
}

/*
typedef struct __attribute__ ((packed)) {
    uint16_t   signature;
    uint8_t   ver1[2];
    uint32_t  serial;
    uint8_t   flags[2];
    char      model[6];
    //uint8_t   fill[2];
} unipiversion_t;


unipiversion_t global_unipi_version;
char  unipi_name[sizeof(global_unipi_version.model)+1];
int unipi_loaded = 0;

int read_unipi1_eprom( unipiversion_t *ver)
{
   int res;
   int i;

   int f = open("/sys/bus/i2c/devices/1-0050/eeprom", O_RDONLY);
   if (f < 0) {
         return 1;
   }
   res = lseek(f, 0xe0, 0);
   if (res < 0) goto err;
   res = read(f,ver,sizeof(unipiversion_t));
   if (res < 0) goto err;
   if (ver->signature != 0x55fa) goto err;
   close(f);
   for (i=0; i<6; i++) {
       if (ver->model[i]==0xff) {
          ver->model[i] = '\0';
          break;
       }
   }
   unipi_loaded = 1;
   return 0;
err:
   close(f);
   return 1;
}

int read_unipi_eprom( unipiversion_t *ver)
{
   int res;
   int i;

   unipi_loaded = 1;
   int f = open("/sys/bus/i2c/devices/1-0057/eeprom", O_RDONLY);
   if (f < 0) {
      f = open("/sys/bus/i2c/devices/0-0057/eeprom", O_RDONLY);
      if (f < 0) {
         f = open("/sys/bus/i2c/devices/2-0057/eeprom", O_RDONLY);
         if (f < 0) {
             return 1;
         }
      }
   }
   res = lseek(f, 0x60, 0);
   if (res < 0) goto err;
   res = read(f,ver,sizeof(unipiversion_t));
   if (res < 0) goto err;
   if (ver->signature != 0x55fa) goto err;
   close(f);
   for (i=0; i<6; i++) {
       if (ver->model[i]==0xff) {
          ver->model[i] = '\0';
          break;
       }
   }
   return 0;
err:
   close(f);
   return 1;
}
*/

/*
char* get_unipi_name(void)
{
	if (! unipi_loaded) {
		if (read_unipi1_eprom(&global_unipi_version) == 0) {
			memcpy(unipi_name, global_unipi_version.model, sizeof(global_unipi_version.model));
			unipi_name[sizeof(global_unipi_version.model)] = '\0';
			if ((unipi_name[0] == '\0') || ((uint8_t)unipi_name[0] == 0xff)) {
				strcpy(unipi_name, "UNIPI1");
			}
		} else if (read_unipi_eprom(&global_unipi_version) == 0) {
			memcpy(unipi_name, global_unipi_version.model, sizeof(global_unipi_version.model));
			unipi_name[sizeof(global_unipi_version.model)] = '\0';

		} else {
			unipi_name[0] = '\0';
		}
	}
	return unipi_name;
}
*/
