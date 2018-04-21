/**
 * The following is the code for react-native app
 */
// Libraries!!
import React, { Component } from 'react';
import {
  StyleSheet,
  Text,
  View,
  Image,
  TouchableOpacity,
  LayoutAnimation,
  NativeModules,
  Alert,
  Button,
  ActivityIndicator,
  FlatList,
  RefreshControl,
  ListView,
  Linking,
  Easing
} from 'react-native';
import ZoomImage from 'react-native-zoom-image';
import { Ionicons, MaterialCommunityIcons } from '@expo/vector-icons';
import {
  StackNavigator,
  TabNavigator,
  TabBarBottom } from 'react-navigation';
import {
  Constants,
  AppLoading,
  Font,
} from 'expo';

const { UIManager } = NativeModules;
UIManager.setLayoutAnimationEnabledExperimental &&
UIManager.setLayoutAnimationEnabledExperimental(true);


/**
 * The main screen that holds the history of all the post on the server
 * Each post has a title, description and also the image
 * @extends Component
 */
class Home extends Component {
  constructor(props){
      super(props);
      this.state ={ isLoading: true, refreshing: false};
    }
    _onRefresh() {
      this.setState({refreshing: true});
      fetch('http://griin.today/API/notifications')
        .then((response) => response.json())
        .then((responseJson) => {

          this.setState({
            isLoading: false,
            dataSource: responseJson.entries,
          }, function(){

          });

        }).then(() => {
        this.setState({refreshing: false});
      });
    }

    componentDidMount(){
      return fetch('http://griin.today/API/notifications')
        .then((response) => response.json())
        .then((responseJson) => {

          this.setState({
            isLoading: false,
            dataSource: responseJson.entries,
          }
          , function(){

          });

        })
        .catch((error) =>{
          console.error(error);
        });
    }
    ListViewItemSeparator = () => {
   return (
     <View
       style={{
         height: .5,
         width: "100%",
         backgroundColor: "#000",
       }}
     />
   );
}
render(){
  if(this.state.isLoading){
    return(
      <View style={{flex: 1, padding: 20}}>
        <ActivityIndicator/>
      </View>
    )
  }
  return (
  <View style={styles.container}>
      <Text style = {styles.text}>RoboCop</Text>
      <FlatList
        refreshControl={
          <RefreshControl
            refreshing={this.state.refreshing}
            onRefresh={this._onRefresh.bind(this)}
          />
        }
        data={this.state.dataSource}
        renderItem={({item}) =>(
          <View key={item.id}>
          <Text style = {styles.title}>{item.title} </Text>
          <Text style = {styles.description}>{item.description} </Text>
          <Image source = {{uri : item.img}} style={styles.imageViewContainer}/>
          </View>
        )}
        keyExtractor={(item, index) => index}
      />
   </View>
);
}
}
/** This is just a screen with one button
 *  This button is used to open the main website on the browser
 * @extends React
 */
class SoundScreen extends Component {
  _onPressButton() {
  Linking.openURL('http://griin.today');
  }

  render() {
    return (
      <View style={{ flex: 1, alignItems: 'center',backgroundColor:'#ff7d', justifyContent: 'center' }}>
      <TouchableOpacity onPress={this._onPressButton}>
          <View style={styles.button}>
            <Text style={styles.buttonText}>Start mapping on your phone!</Text>
          </View>
      </TouchableOpacity>

      </View>
    );
  }
}
/**
 * This is the picture screen which has 2 buttons
 * and a view of the pictures on the server
 * @extends Component
 */

class DetailsScreen extends Component {
  constructor(props){
      super(props);
      this.state ={ isLoading: true, refreshing: false};
    }
    _onRefresh() {
      this.setState({refreshing: true});
      fetch('http://griin.today/API/notifications')
        .then((response) => response.json())
        .then((responseJson) => {

          this.setState({
            isLoading: false,
            dataSource: responseJson.entries,
          }, function(){

          });

        }).then(() => {
        this.setState({refreshing: false});
      });
    }

    componentDidMount(){
      return fetch('http://griin.today/API/notifications')
        .then((response) => response.json())
        .then((responseJson) => {

          this.setState({
            isLoading: false,
            dataSource: responseJson.entries,
          }
          , function(){

          });

        })
        .catch((error) =>{
          console.error(error);
        });
    }
    // Button to send picture request
    _onPressButton() {
    Alert.alert('Please refresh and allow up to 10 seconds for the picture to be uploaded.');
    fetch('http://griin.today/API/picture_request', {
    method: 'POST'
    });
    }
      // Button to send delete request
    _onPressClearButton() {
    fetch('http://griin.today/API/notifications', {
    method: 'DELETE'
    });
    }

render(){
  if(this.state.isLoading){
    return(
      <View style={{flex: 1, padding: 20}}>
        <ActivityIndicator/>
      </View>
    )
  }
  return (
  <View style={styles.container}>
  <TouchableOpacity onPress={this._onPressButton}>
      <View style={styles.button}>
        <Text style={styles.buttonText}>Take a Pic!</Text>
      </View>
  </TouchableOpacity>

  <TouchableOpacity onPress={this._onPressClearButton}>
      <View style={styles.button}>
        <Text style={styles.buttonText}>Clear All!</Text>
      </View>
  </TouchableOpacity>

      <FlatList
        refreshControl={
          <RefreshControl
            refreshing={this.state.refreshing}
            onRefresh={this._onRefresh.bind(this)}
          />
        }
        data={this.state.dataSource}
        renderItem={({item}) =>(
          <View key={item.id}>
          <ZoomImage
  source={{uri: item.img}}
  imgStyle={{width: 250, height: 230}}
  style={styles.JustImageViewContainer}
  duration={200}
  enableScaling={false}
  easingFunc={Easing.ease}
/>

          </View>
        )}
        keyExtractor={(item, index) => index}
      />
   </View>
);
}
}
/**
 * The defualt class that controls the transition between different apps
 * There are 3 main Navigation tabs: Website, Home, and picture
 */
export default TabNavigator(
  {
    Home: { screen: Home },
    Picture: { screen: DetailsScreen },
    Website: {screen: SoundScreen}
  },
  {
    navigationOptions: ({ navigation }) => ({
      tabBarIcon: ({ focused, tintColor }) => {
        const { routeName } = navigation.state;
        let iconName;
        if (routeName === 'Home') {
          iconName = `ios-ionitron${focused ? '' : '-outline'}`;
        } else if (routeName === 'Picture') {
          iconName = `ios-camera${focused ? '' : '-outline'}`;
        }
        else if (routeName === 'Website'){
          iconName = `ios-globe${focused ? '':'-outline'}`;
        }

        // icon component from react-native-vector-icons
        return <Ionicons name={iconName} size={30} color={tintColor} />;
      },
    }),
    tabBarComponent: TabBarBottom,
    tabBarPosition: 'bottom',
    tabBarOptions: {
      activeTintColor: 'orange',
      inactiveTintColor: 'gray',
    },
    animationEnabled: false,
    swipeEnabled: true,
  }
);

/**
 * Style sheets for all most components are here
 *
 */
const styles = StyleSheet.create({
container: {
flex: 1,
paddingTop: Constants.statusBarHeight,
backgroundColor: '#ff7d',
borderColor: 'white',
alignItems: 'center',
borderWidth: 1,
},
text: {
color: 'black',
fontSize:50
},
button: {
backgroundColor: 'black',
borderRadius: 50,
paddingHorizontal: 20,
paddingVertical: 15,
marginTop: 15,
},
imageViewContainer: {
height: 150 ,
margin: 10,
borderRadius : 10
},
JustImageViewContainer: {
height: 150 ,
width:250,
margin: 10,
borderRadius : 10
},
buttonText: {
color: 'white',
fontWeight: 'bold',
},
description:{
fontSize: 14,
backgroundColor: '#ff70'
},
title: {
fontSize: 16,
fontWeight: 'bold',
backgroundColor: '#ff70'
}
});
