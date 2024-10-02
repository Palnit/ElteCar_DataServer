<h1 align=center>Elte Car</h1>

<p>This project loads the data from Eötvös Lorand University's self-driving car's camera and lidar data into shared memory</p>
<p>Usage:</p>
<table>
<tr>
<th>Switch</th>
<th>Description</th>
<th>Expected data</th>
</tr>
<tr>
<td>-d/--delay</td>
<td>Delays the data loading for that many milliseconds</td>
<td>Number</td>
</tr>
<tr>
<td>-m/--max</td>
<td>The maximum number of data points if a data point is missing its kips it</td>
<td>Number</td>
</tr>
<tr>
<td>-i/--images</td>
<td>list of all image paths the first value must be the base of all images then only the file name of the image containing a {} where the numbering is supposed to go it will be changed for the number in the file</td>
<td>Array of strings</td>
</tr>
<tr>
<td>-c/--csvpath</td>
<td>The path to the csv containing cartesians data must be a full path</td>
<td>string</td>
</tr>
<tr>
<td>-l/--lidar</td>
<td>The full path to the lidar data with a base file name containing {} wher the numbering goes the {} will be changed for the number of the file</td>
<td>string</td>
</tr>
</table>
